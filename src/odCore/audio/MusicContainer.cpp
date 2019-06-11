/*
 * MusicContainer.cpp
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#include <odCore/audio/MusicContainer.h>

#include <odCore/SrscRecordTypes.h>

namespace odAudio
{

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
            }

            cursor.nextOfType(od::SrscRecordType::MUSIC);
        }
    }

    void MusicContainer::_addDlsToIndex(RiffReader rr, od::RecordId id)
    {
        rr.skipToFirstSubchunk();
        rr.nextChunkOfType(RiffReader::LIST_CHUNK_ID, "INFO");
        if(rr.isEnd())
        {
            Logger::warn() << "No INFO chunk in DLS file/record. Can't index DLS file";
            return;
        }

        rr.skipToFirstSubchunk();

        // ...
    }

}
