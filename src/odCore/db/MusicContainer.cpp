/*
 * MusicContainer.cpp
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#include <odCore/db/MusicContainer.h>

#include <limits>

#include <odCore/Panic.h>
#include <odCore/SrscRecordTypes.h>

#include <odCore/db/Segment.h>

namespace odDb
{

    MusicContainer::MusicContainer(const od::FilePath &musicContainerFile)
    : mFile(musicContainerFile)
    , mRrc(musicContainerFile)
    {
        Logger::info() << "Loading music container " << mFile;
        _buildIndex();
    }

    od::RecordId MusicContainer::getDlsRecordByGuid(const od::Guid &guid)
    {
        auto it = mDlsGuidMap.find(guid);
        if(it == mDlsGuidMap.end())
        {
            OD_PANIC() << "No DLS with given GUID " << guid << " found";
        }

        return it->second;
    }

    od::SrscFile::RecordInputCursor MusicContainer::getCursorForDls(const od::Guid &guid)
    {
        od::RecordId id = getDlsRecordByGuid(guid);
        return mRrc.getFirstRecordOfTypeId(od::SrscRecordType::MUSIC, id);
    }

    std::shared_ptr<Segment> MusicContainer::loadSegment(MusicId id)
    {
        if(id > std::numeric_limits<od::RecordId>::max())
        {
            OD_PANIC() << "Music ID out of record ID limits: " << id;
        }

        auto cursor = mRrc.getFirstRecordOfTypeId(od::SrscRecordType::MUSIC, static_cast<od::RecordId>(id));
        if(!cursor.isValid())
        {
            OD_PANIC() << "Music with ID " << id << " not found";
        }

        Logger::verbose() << "Loading music segment " << std::hex << id << std::dec;

        od::RiffReader rr(cursor.getReader());
        auto segment = std::make_shared<Segment>(rr);
        return segment;
    }

    void MusicContainer::_buildIndex()
    {
        auto cursor = mRrc.getFirstRecordOfType(od::SrscRecordType::MUSIC);
        while(cursor.isValid())
        {
            auto readerOption = od::RiffReader::create(cursor.getReader());
            if(readerOption.has_value())
            {
                auto &riffReader = *readerOption;
                if(riffReader.getListId() == "DLS ")
                {
                    _addDlsToIndex(riffReader, cursor.getDirIterator()->recordId);

                }else if(riffReader.getListId() == "DMSG")
                {
                    _addSegmentToIndex(riffReader, cursor.getDirIterator()->recordId);
                }
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

                name.resize(nameReader.getChunkLength()-1, ' ');
                nameReader.getDataReader().read(&name[0], nameReader.getChunkLength()-1);
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
            Logger::verbose() << "Added DLS with GUID=" << dlid << " and name '" << name << "' to index";

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
        //Segment segment(rr);
    }

}
