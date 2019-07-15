/*
 * SegmentPlayer.cpp
 *
 *  Created on: Jun 18, 2019
 *      Author: zal
 */

#include <odCore/audio/music/SegmentPlayer.h>

#include <algorithm>
#include <cmath>

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#include <odCore/db/Segment.h>

#include <odCore/audio/music/MidiSynth.h>
#include <odCore/audio/music/DMusicDefines.h>

namespace odAudio
{

    SegmentPlayer::SegmentPlayer(MidiSynth &synth)
    : mSynth(synth)
    , mTempoBps(25.0/60.0)
    , mPlaying(false)
    , mCurrentMusicTime(0)
    , mNoteIterator(mNoteEvents.end())
    {
    }

    SegmentPlayer::~SegmentPlayer()
    {
    }

    void SegmentPlayer::setSegment(odDb::Segment *s)
    {
        mSynth.allNotesOff();

        mSegment = s;
        mNoteEvents.clear();
        mActiveCurves.clear();
        mNoteIterator = mNoteEvents.end();

        if(mSegment == nullptr)
        {
            return;
        }

        auto &midiEvents = mSegment->getMidiEvents();
        // note events turn into two events. all others stay the same. but since most events are note events, we don't waste too much space
        mNoteEvents.reserve(midiEvents.size()*2);
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

        mCurveIterator = mSegment->getMidiCurves().begin();
        mBandIterator = mSegment->getBandEvents().begin();
        mTempoIterator = mSegment->getTempoEvents().begin();

        // preload all used DLSs so we don't get skipping music due to DLS loading
        // TODO: maybe let the sound system handle this. there is more resource management involved than just preloading DLSs (unloading etc.)
        for(auto &dlsGuid : mSegment->getDlsGuids())
        {
            mSynth.preloadDls(dlsGuid);
        }

        update(0.0f);
    }

    void SegmentPlayer::play()
    {
        mPlaying = true;
    }

    void SegmentPlayer::pause()
    {
        mPlaying = false;
    }

    void SegmentPlayer::update(float relTime)
    {
        if(!mPlaying || mSegment == nullptr)
        {
            return;
        }

        double musicTimePassed = relTime*mTempoBps*MUSICTIME_TICKS_PER_QUARTER;// note: "beats" in bpm seems to refer to quarternotes for some reason
        mCurrentMusicTime += musicTimePassed;

        // first, process single shot events (tempo, band and midi events). we apply all unprocessed events that lie in the past at once.

        while(mTempoIterator != mSegment->getTempoEvents().end() && mTempoIterator->time <= mCurrentMusicTime)
        {
            mTempoBps = mTempoIterator->tempo / 60.0; // got bpm. we want bps
            ++mTempoIterator;
        }

        while(mBandIterator != mSegment->getBandEvents().end() && mBandIterator->time <= mCurrentMusicTime)
        {
            // TODO: we only need to apply the most recent band
            _applyBand(mBandIterator->band.get());
            ++mBandIterator;
        }

        while(mNoteIterator != mNoteEvents.end() && mNoteIterator->time <= mCurrentMusicTime)
        {
            if(mNoteIterator->on)
            {
                mSynth.noteOn(mNoteIterator->channel, mNoteIterator->note, mNoteIterator->velocity);

            }else
            {
                mSynth.noteOff(mNoteIterator->channel, mNoteIterator->note);
            }

            ++mNoteIterator;
        }


        // now handle curves. these aren't single shot, but stay active over multiple updates. thus, keep a deque of active curves
        //  that is updated periodically. first, add new curves to the top of the curve deque
        auto hasStarted = [this](const odDb::Segment::MidiCurve &curve) { return curve.startTime <= mCurrentMusicTime; };
        while(mCurveIterator != mSegment->getMidiCurves().end() && hasStarted(*mCurveIterator))
        {
            mActiveCurves.push_back(*mCurveIterator);
            ++mCurveIterator;
        }

        // apply all active curves in the deque (expired curves still get processed, but their values clamped to the end value)
        for(auto &curve : mActiveCurves)
        {
            _applyCurve(curve);
        }

        // finally, remove expired curves from the bottom of the deque
        //  ńote that since we sort by start time, this might leave some expired curves in the list if they are predeced by
        //  a longer lasting curve that is still active. this is not a major concern, as expired curves are still handled
        //  gracefully and will eventually be removed in any case.
        auto hasEnded = [this](const odDb::Segment::MidiCurve &curve){ return (curve.startTime+curve.duration) <= mCurrentMusicTime; };
        while(!mActiveCurves.empty() && hasEnded(mActiveCurves.front()))
        {
            mActiveCurves.pop_front();
        }
    }

    void SegmentPlayer::_applyCurve(const odDb::Segment::MidiCurve &curve)
    {
        double delta = (mCurrentMusicTime - curve.startTime)/curve.duration;
        delta = glm::clamp(delta, 0.0, 1.0);

        // curve shape approximations, taken from libdmusic
        double deltaCurve;
        switch(curve.curveShape)
        {
        case directmusic::DMUS_CURVES_LINEAR:
            deltaCurve = delta;
            break;

        case directmusic::DMUS_CURVES_INSTANT:
            deltaCurve = 1.0;
            break;

        case directmusic::DMUS_CURVES_EXP:
            deltaCurve = delta*delta*delta*delta;
            break;

        case directmusic::DMUS_CURVES_LOG:
            deltaCurve = std::sqrt(delta);
            break;

        case directmusic::DMUS_CURVES_SINE:
            deltaCurve = std::sin((delta - 0.5) * glm::pi<double>()) + 1;
            break;

        default:
            throw od::Exception("Unknown curve shape");
        }

        int32_t curveValue = curve.startValue*(1-deltaCurve) + curve.endValue*deltaCurve;

        switch(curve.type)
        {
        case directmusic::DMUS_CURVET_PBCURVE: // pitch bend
            mSynth.pitchBend(curve.channelIndex, curveValue);
            break;

        case directmusic::DMUS_CURVET_CCCURVE: // continous controller
            mSynth.controllerChange(curve.channelIndex, curve.ccData, curveValue);
            break;

        case directmusic::DMUS_CURVET_MATCURVE: // monophonic aftertouch
            mSynth.channelPressure(curve.channelIndex, curveValue);
            break;

        case directmusic::DMUS_CURVET_PATCURVE: // polyphonic aftertouch
            // there is no documentation on where to get the key this poly aftertouch affects, but
            // the only viable option i see is ccData. the alternative is to not support poly aftertouch.
            mSynth.keyPressure(curve.channelIndex, curve.ccData, curveValue);
            break;

        case directmusic::DMUS_CURVET_RPNCURVE: // registered parameter number curve (probably DX8)
        case directmusic::DMUS_CURVET_NRPNCURVE:  // non-registered parameter number curve (probably DX8)
        default:
           throw od::Exception("Unsupported/unknown curve type");
        }
    }

    void SegmentPlayer::_applyBand(odDb::Band *band)
    {
        if(band == nullptr)
        {
            return;
        }

        for(auto &instrument : band->getInstruments())
        {
            mSynth.assignPreset(instrument.pChannel, 0, instrument.patch, instrument.dlsGuid);
        }
    }
}


