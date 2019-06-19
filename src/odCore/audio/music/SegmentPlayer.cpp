/*
 * SegmentPlayer.cpp
 *
 *  Created on: Jun 18, 2019
 *      Author: zal
 */

#include <odCore/audio/music/SegmentPlayer.h>

#include <algorithm>

#include <odCore/audio/music/Segment.h>
#include <odCore/audio/music/MidiSynth.h>


namespace odAudio
{

    SegmentPlayer::SegmentPlayer(MidiSynth *synth)
    : mSynth(synth)
    , mTempo(1.0f)
    , mCurrentMusicTime(0)
    {
    }

    void SegmentPlayer::setSegment(std::unique_ptr<Segment> &&s)
    {
        mSegment = s;

        mNoteEvents.clear();
        auto &midiEvents = mSegment->getMidiEvents();
        for(auto &event : midiEvents)
        {
            switch(event.midiStatus)
            {
            case 0x90: // note-on
                {
                    NoteEvent note;
                    note.on = true;
                    note.channel = event.channelIndex;
                    note.time = event.startTime;
                    note.note = event.midiData1;
                    note.velocity = event.midiData2;
                    mNoteEvents.push_back(note);

                    note.on = false;
                    note.time += event.duration;
                    mNoteEvents.push_back(note);
                }
                break;

            default:
                break;
            }
        }

        auto pred = [](const NoteEvent &lhs, const NoteEvent &rhs){ return lhs.time < rhs.time; };
        std::sort(mNoteEvents.begin(), mNoteEvents.end(), pred);

        mNoteIterator = mNoteEvents.begin();
    }

    void SegmentPlayer::play()
    {
    }

    void SegmentPlayer::pause()
    {
    }

    void SegmentPlayer::update(float relTime)
    {
        mCurrentMusicTime += (relTime*1000)*mTempo;

        while(mNoteIterator != mNoteEvents.end() && mNoteIterator->time < mCurrentMusicTime)
        {
            if(mNoteIterator->on)
            {
                mSynth->noteOn(mNoteIterator->channel, mNoteIterator->note, mNoteIterator->velocity);

            }else
            {
                mSynth->noteOff(mNoteIterator->channel, mNoteIterator->note);
            }

            ++mNoteIterator;
        }
    }

}


