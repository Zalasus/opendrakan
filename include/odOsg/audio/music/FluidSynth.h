/*
 * FluidSynth.h
 *
 *  Created on: Jun 18, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_MUSIC_FLUIDSYNTH_H_
#define INCLUDE_ODOSG_AUDIO_MUSIC_FLUIDSYNTH_H_

#include <odCore/audio/music/MidiSynth.h>

namespace odOsg
{

    class FluidSynth : public odAudio::MidiSynth
    {
    public:

        FluidSynth();

        virtual void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
        virtual void noteOff(uint8_t channel, uint8_t note) override;

    };

}


#endif /* INCLUDE_ODOSG_AUDIO_MUSIC_FLUIDSYNTH_H_ */
