/*
 * MusicContainer.cpp
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#include <odCore/db/MusicContainer.h>

#include <limits>

#include <odCore/SrscRecordTypes.h>

#include <odCore/db/Segment.h>

namespace odDb
{

    MusicContainer::MusicContainer(const od::FilePath &musicContainerFile)
    : mFile(musicContainerFile)
    , mRrc(musicContainerFile)
    {
        _buildIndex();
    }

    od::RecordId MusicContainer::getDlsRecordByGuid(const od::Guid &guid)
    {
        auto it = mDlsGuidMap.find(guid);
        if(it == mDlsGuidMap.end())
        {
            throw od::NotFoundException("No DLS with given GUID found");
        }

        return it->second;
    }

    od::RefPtr<Segment> MusicContainer::loadSegment(MusicId id)
    {
        if(id > std::numeric_limits<od::RecordId>::max())
        {
            throw od::Exception("Music ID out of record ID limits");
        }

        auto cursor = mRrc.getFirstRecordOfTypeId(od::SrscRecordType::MUSIC, static_cast<od::RecordId>(id));
        if(!cursor.isValid())
        {
            throw od::NotFoundException("Music with given ID not found");
        }

        od::RiffReader rr(cursor.getReader());
        auto segment = od::make_refd<Segment>(rr);
        return segment;
    }

    void MusicContainer::_buildIndex()
    {
        auto cursor = mRrc.getFirstRecordOfType(od::SrscRecordType::MUSIC);
        while(cursor.isValid())
        {
            try
            {
                od::RiffReader riffReader(cursor.getReader());

                if(riffReader.getListId() == "DLS ")
                {
                    _addDlsToIndex(riffReader, cursor.getDirIterator()->recordId);

                }else if(riffReader.getListId() == "DMSG")
                {
                    _addSegmentToIndex(riffReader, cursor.getDirIterator()->recordId);
                }

            }catch(od::RiffException &r)
            {
                // if the record contains no RIFF data, simply ignore it
            }

            cursor.nextOfType(od::SrscRecordType::MUSIC);
        }
    }

    void MusicContainer::_addDlsToIndex(od::RiffReader rr, od::RecordId id)
    {
        rr.skipToFirstSubchunk();

        bool gotDlid = false;
        bool gotName = false;
        std::string name;
        od::Guid dlid;
        while(!rr.isEnd())
        {
            if(rr.getChunkId() == "dlid")
            {
                if(gotDlid)
                {
                    Logger::warn() << "Double dlid chunk in DLS record. Ignoring";
                    continue;
                }

                dlid = od::Guid(rr);
                gotDlid = true;

            }else if(rr.getListId() == "INFO")
            {
                if(gotName)
                {
                    Logger::warn() << "Double INFO chunk in DLS record. Ignoring";
                    continue;
                }

                od::RiffReader nameReader = rr.getReaderForFirstSubchunkOfType("INAM");
                if(nameReader.isEnd())
                {
                    Logger::warn() << "DLS record has no INAM subchunk in INFO chunk";
                    break;
                }

                name.resize(nameReader.getChunkLength(), ' ');
                nameReader.getDataReader().read(&name[0], 16);
                gotName = true;
            }

            if(gotDlid && gotName)
            {
                break;
            }

            rr.skipToNextChunk();
        }

        if(gotDlid && gotName)
        {
            auto it = mDlsGuidMap.find(dlid);
            if(it != mDlsGuidMap.end())
            {
                Logger::warn() << "Ignoring duplicate DLS with GUID=" << dlid << " and name '" << name << "'";
                return;
            }

            mDlsGuidMap[dlid] = id;
            Logger::verbose() << "Added DLS '" << name << "' with GUID=" << dlid << " to index";

        }else if(!gotDlid)
        {
            Logger::warn() << "DLS had no valid GUID";

        }else if(!gotName)
        {
            Logger::warn() << "DLS had no valid name";
        }
    }

    void MusicContainer::_addSegmentToIndex(od::RiffReader rr, od::RecordId id)
    {
        Segment segment(rr);
    }

}
