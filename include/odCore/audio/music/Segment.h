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
            static const size_t STRUCT_BYTES = 17;

            uint32_t startTime; // MUSIC_TIME
            uint32_t duration;  // MUSIC_TIME
            uint32_t channelIndex;
            int16_t timeOffset;
            uint8_t midiStatus;
            uint8_t midiData1;
            uint8_t midiData2;
        };

        struct MidiCurve
        {
            static const size_t STRUCT_BYTES = 17;

            uint32_t startTime;
            uint32_t duration;
            uint32_t resetDuration;
            uint32_t channelIndex;
            int16_t timeOffset;
            int16_t startValue;
            int16_t endValue;
            int16_t resetValue;
            uint8_t type;
            uint8_t curveShape;
            uint8_t ccData;
            uint8_t flags;
            uint16_t paramType;
            uint16_t mergeIndex;
        };

        Segment(RiffReader rr);

        inline Guid getGuid() const { return mGuid; }
        inline const std::vector<MidiEvent> &getMidiEvents() const { return mMidiEvents; }


    private:

        void _load(RiffReader rr);
        void _loadHeader(RiffReader rr);
        void _loadTracklist(RiffReader rr);
        void _loadSequenceTrack(RiffReader rr);
        void _loadBandTrack(RiffReader rr);

        Guid mGuid;

        std::vector<MidiEvent> mMidiEvents;
        std::vector<MidiCurve> mMidiCurves;

    };

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENT_H_ */
