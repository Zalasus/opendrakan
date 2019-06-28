/*
 * FluidSynth.h
 *
 *  Created on: Jun 18, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_MUSIC_FLUIDSYNTH_H_
#define INCLUDE_ODOSG_AUDIO_MUSIC_FLUIDSYNTH_H_

#include <fluidsynth/types.h>
#include <fluidsynth/synth.h>

#include <odCore/audio/music/MidiSynth.h>

namespace odOsg
{

    class FluidSynth : public odAudio::MidiSynth
    {
    public:

        FluidSynth();
        virtual ~FluidSynth();

        virtual void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
        virtual void noteOff(uint8_t channel, uint8_t note) override;

        virtual void fillInterleavedStereoBuffer(int16_t *buffer, size_t size) override;


    private:

        fluid_synth_t *mSynth;

    };

}


#endif /* INCLUDE_ODOSG_AUDIO_MUSIC_FLUIDSYNTH_H_ */
