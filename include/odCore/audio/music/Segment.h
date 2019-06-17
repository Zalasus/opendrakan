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

        struct MidiEvent
        {
            uint32_t startTime;
            uint32_t duration;
            uint32_t channelIndex;
            uint8_t command;
            uint8_t commandArg1;
            uint8_t commandArg2;
        };

        Segment(RiffReader rr);

        inline Guid getGuid() const { return mGuid; }


    private:

        void _load(RiffReader rr);
        void _loadHeader(RiffReader rr);
        void _loadTracklist(RiffReader rr);
        void _loadSequenceTrack(RiffReader rr);

        Guid mGuid;

        std::vector<MidiEvent> mMidiEvents;

    };

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENT_H_ */
