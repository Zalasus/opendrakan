/*
 * SegmentPlayer.h
 *
 *  Created on: Jun 18, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENTPLAYER_H_
#define INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENTPLAYER_H_

#include <vector>
#include <deque>
#include <memory>

#include <odCore/CTypes.h>

#include <odCore/db/Segment.h>

namespace odAudio
{
    class MidiSynth;

    class SegmentPlayer
    {
    public:

        /**
         * @brief The number of ticks added to music time in the duration of a quarter note.
         *
         * As defined by the DirectMusic specs (in constant DMUS_PPQ).
         */
        static const size_t MUSICTIME_TICKS_PER_QUARTER = 768;

        SegmentPlayer(MidiSynth &synth);
        ~SegmentPlayer();

        void setSegment(std::shared_ptr<odDb::Segment> s);

        void play();
        void pause();

        void update(float relTime);


    private:

        void _applyCurve(const odDb::Segment::MidiCurve &curve);
        void _applyBand(odDb::Band *band);

        struct NoteEvent
        {
            uint32_t time;
            bool on;
            uint8_t note;
            uint8_t channel;
            uint8_t velocity;
        };

        MidiSynth &mSynth;

        std::shared_ptr<odDb::Segment> mSegment;
        std::vector<NoteEvent> mNoteEvents;

        double mTempoBps; // beats per second
        bool mPlaying;
        double mCurrentMusicTime;
        std::vector<NoteEvent>::const_iterator mNoteIterator;

        odDb::Segment::CurveVector::const_iterator mCurveIterator;
        std::deque<odDb::Segment::MidiCurve> mActiveCurves;

        odDb::Segment::TempoVector::const_iterator mTempoIterator;

        odDb::Segment::BandVector::const_iterator mBandIterator;

    };

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_SEGMENTPLAYER_H_ */
