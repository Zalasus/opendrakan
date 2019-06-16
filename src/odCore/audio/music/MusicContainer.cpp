/*
 * MusicContainer.cpp
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#include <odCore/audio/music/MusicContainer.h>

#include <odCore/SrscRecordTypes.h>

#include <odCore/audio/music/Segment.h>

namespace odAudio
{

    MusicContainer::MusicContainer(const od::FilePath &musicContainerFile)
    : mFile(musicContainerFile)
    , mRrc(musicContainerFile)
    {
        _buildIndex();
    }

    void MusicContainer::_buildIndex()
    {
        auto cursor = mRrc.getFirstRecordOfType(od::SrscRecordType::MUSIC);
        while(cursor.isValid())
        {
            try
            {
                RiffReader riffReader(cursor.getReader());

                if(riffReader.getListId() == "DLS ")
                {
                    _addDlsToIndex(riffReader, cursor.getDirIterator()->recordId);

                }else if(riffReader.getListId() == "DMSG")
                {
                    _addSegmentToIndex(riffReader, cursor.getDirIterator()->recordId);
                }

            }catch(RiffException &r)
            {
            }

            cursor.nextOfType(od::SrscRecordType::MUSIC);
        }
    }

    void MusicContainer::_addDlsToIndex(RiffReader rr, od::RecordId id)
    {
        rr.skipToFirstSubchunk();

        bool gotDlid = false;
        bool gotName = false;
        std::string name;
        Guid dlid;
        while(!rr.isEnd())
        {
            if(rr.getChunkId() == "dlid")
            {
                if(gotDlid)
                {
                    Logger::warn() << "Double dlid chunk in DLS record. Ignoring";
                    continue;
                }

                dlid = Guid(rr);
                gotDlid = true;

            }else if(rr.getListId() == "INFO")
            {
                if(gotName)
                {
                    Logger::warn() << "Double INFO chunk in DLS record. Ignoring";
                    continue;
                }

                RiffReader nameReader = rr.getReaderForFirstSubchunkOfType(FourCC("INAM"));
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

    void MusicContainer::_addSegmentToIndex(RiffReader rr, od::RecordId id)
    {
        Segment segment(rr);
    }

}
