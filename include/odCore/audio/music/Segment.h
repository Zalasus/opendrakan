/*
 * Segment.h
 *
 *  Created on: Jun 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENT_H_
#define INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENT_H_

#include <vector>

#include <odCore/audio/music/RiffReader.h>
#include <odCore/audio/music/Guid.h>

namespace odAudio
{

    class Segment
    {
    public:

        Segment(RiffReader rr);

        inline Guid getGuid() const { return mGuid; }


    private:

        void _load(RiffReader rr);
        void _loadHeader(RiffReader rr);
        void _loadTracklist(RiffReader rr);

        Guid mGuid;

    };

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENT_H_ */
