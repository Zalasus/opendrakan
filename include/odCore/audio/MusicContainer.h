/*
 * MusicContainer.h
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_MUSICCONTAINER_H_
#define INCLUDE_ODCORE_AUDIO_MUSICCONTAINER_H_

#include <map>
#include <string>

#include <odCore/SrscFile.h>

#include <odCore/audio/RiffReader.h>

namespace odAudio
{

    /**
     * @brief Helper class for reading and indexing Music.rrc and for providing music data to the music system.
     */
    class MusicContainer
    {
    public:

        typedef uint32_t MusicId;


        MusicContainer(const od::FilePath &musicContainerFile);

        od::RecordId getSegmentRecordId(MusicId musicId);

        od::DataReader getReaderForDls();


    private:

        void _buildIndex();
        void _addDlsToIndex(RiffReader rr, od::RecordId id);

        od::SrscFile mRrc;

        std::map<std::string, od::RecordId> mDlsMap;
    };

}

#endif /* INCLUDE_ODCORE_AUDIO_MUSICCONTAINER_H_ */
