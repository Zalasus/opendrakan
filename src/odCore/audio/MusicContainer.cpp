/*
 * MusicContainer.cpp
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#include <odCore/audio/MusicContainer.h>

#include <cstring>

#include <odCore/SrscRecordTypes.h>

namespace odAudio
{

    bool MusicContainer::Guid::operator<(const Guid &rhs) const
    {
        return std::memcmp(data.data(), rhs.data.data(), LENGTH) < 0;
    }

    bool MusicContainer::Guid::operator==(const Guid &rhs) const
    {
        return std::memcmp(data.data(), rhs.data.data(), LENGTH) == 0;
    }


    MusicContainer::MusicContainer(const od::FilePath &musicContainerFile)
    : mRrc(musicContainerFile)
    {
        _buildIndex();
    }

    void MusicContainer::_buildIndex()
    {
        auto cursor = mRrc.getFirstRecordOfType(od::SrscRecordType::MUSIC);
        while(cursor.isValid())
        {
            RiffReader riffReader(cursor.getReader());

            if(riffReader.getListId() == "DLS ")
            {
                _addDlsToIndex(riffReader, cursor.getDirIterator()->recordId);

            }else if(riffReader.getListId() == "DMSG")
            {
                _addSegmentToIndex(riffReader, cursor.getDirIterator()->recordId);
            }

            cursor.nextOfType(od::SrscRecordType::MUSIC);
        }
    }

    void MusicContainer::_addDlsToIndex(RiffReader rr, od::RecordId id)
    {
        rr.skipToFirstSubchunk();

        bool gotDlid = false;
        bool gotName = false;
        while(!rr.isEnd())
        {
            if(rr.getChunkId() == "dlid")
            {
                if(gotDlid)
                {
                    Logger::warn() << "Double dlid chunk in DLS record. Ignoring";
                    continue;
                }

                if(rr.getChunkLength() != Guid::LENGTH)
                {
                    Logger::warn() << "DLS record has dlid chunk of invalid length. Can't add DLS to GUID index";
                    continue;
                }

                Guid dlid;
                od::DataReader dr = rr.getReader();
                dr.read(dlid.data.data(), 16);
                mDlsGuidMap[dlid] = id;
                gotDlid = true;

            }else if(rr.getListId() == "INFO")
            {
                if(gotName)
                {
                    Logger::warn() << "Double INFO chunk in DLS record. Ignoring";
                    continue;
                }

                RiffReader nameReader = rr.getReaderForFirstSubchunkOfType("INAM");
                if(nameReader.isEnd())
                {
                    Logger::warn() << "DLS record has no INAM subchunk in INFO chunk. Can't add DLS to name index";
                    continue;
                }

                std::string name;
                name.resize(nameReader.getChunkLength(), ' ');
                nameReader.getReader().read(&name[0], 16);
                mDlsNameMap[name] = id;
                gotName = true;
            }

            rr.skipToNextChunk();
        }
    }

    void MusicContainer::_addSegmentToIndex(RiffReader rr, od::RecordId id)
    {
    }

}
