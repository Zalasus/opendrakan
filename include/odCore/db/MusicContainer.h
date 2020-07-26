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
#include <memory>

#include <odCore/SrscFile.h>
#include <odCore/RiffReader.h>
#include <odCore/Guid.h>

namespace odDb
{
    class Segment;

    /**
     * @brief Helper class for reading and indexing Music.rrc and for providing music data to the music system.
     */
    class MusicContainer
    {
    public:

        typedef uint32_t MusicId;

        MusicContainer(const od::FilePath &musicContainerFile);

        od::RecordId getDlsRecordByGuid(const od::Guid &guid);
        od::SrscFile::RecordInputCursor getCursorForDls(const od::Guid &guid);

        std::shared_ptr<Segment> loadSegment(MusicId id);


    private:

        void _buildIndex();
        void _addDlsToIndex(od::RiffReader rr, od::RecordId id);
        void _addSegmentToIndex(od::RiffReader rr, od::RecordId id);

        od::FilePath mFile;
        od::SrscFile mRrc;

        std::map<od::Guid, od::RecordId> mDlsGuidMap;
    };

}

#endif /* INCLUDE_ODCORE_AUDIO_MUSICCONTAINER_H_ */
