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

        /**
         * @brief The number of ticks added to music time in the duration of a quarter note.
         *
         * As defined by the DirectMusic specs (in constant DMUS_PPQ).
         */
        static const size_t MUSICTIME_TICKS_PER_QUARTER = 768;

        SegmentPlayer(MidiSynth *synth);
        ~SegmentPlayer();

        void setSegment(std::unique_ptr<Segment> s);

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

        MidiSynth *mSynth;

        std::unique_ptr<Segment> mSegment;
        std::vector<NoteEvent> mNoteEvents;

        double mTempoBps; // beats per second
        uint32_t mCurrentMusicTime;
        std::vector<NoteEvent>::iterator mNoteIterator;
    };

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENTPLAYER_H_ */
