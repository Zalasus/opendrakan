/*
 * SegmentPlayer.h
 *
 *  Created on: Jun 18, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENTPLAYER_H_
#define INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENTPLAYER_H_

#include <memory>
#include <vector>

namespace odAudio
{
    class MidiSynth;
    class Segment;

    class SegmentPlayer
    {
    public:

        SegmentPlayer(MidiSynth *synth);

        void setSegment(std::unique_ptr<Segment> &&s);

        void play();
        void pause();

        void update(float relTime);


    private:

        struct NoteEvent
        {
            uint32_t time;
            bool on;
            uint8_t note;
            uint8_t channel;
            uint8_t velocity;
        };

        struct Curve
        {
            uint32_t startTime;
            uint32_t endTime;
            uint8_t controller;
            uint16_t startValue;
            uint16_t endValue;
        };

        MidiSynth *mSynth;

        std::unique_ptr<Segment> mSegment;
        std::vector<NoteEvent> mNoteEvents;

        float mTempo; // in ticks per millisecond
        uint32_t mCurrentMusicTime;
        std::vector<NoteEvent>::iterator mNoteIterator;
    };

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENTPLAYER_H_ */
