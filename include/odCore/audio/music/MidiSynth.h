/*
 * MidiSynth.h
 *
 *  Created on: Jun 18, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_MUSIC_MIDISYNTH_H_
#define INCLUDE_ODCORE_AUDIO_MUSIC_MIDISYNTH_H_

#include <stdint.h>

namespace odAudio
{

    /**
     * @brief Abstract interface for a MIDI snythesizer.
     */
    class MidiSynth
    {
    public:

        virtual ~MidiSynth() = default;

        virtual void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
        virtual void noteOff(uint8_t channel, uint8_t note) = 0;

    };

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_MIDISYNTH_H_ */
