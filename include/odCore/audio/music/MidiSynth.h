/*
 * MidiSynth.h
 *
 *  Created on: Jun 18, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_MUSIC_MIDISYNTH_H_
#define INCLUDE_ODCORE_AUDIO_MUSIC_MIDISYNTH_H_

#include <odCore/CTypes.h>
#include <odCore/Guid.h>

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
        virtual void pitchBend(uint8_t channel, uint16_t value) = 0;
        virtual void controllerChange(uint8_t channel, uint8_t controller, uint8_t value) = 0;
        virtual void channelPressure(uint8_t channel, uint8_t value) = 0;
        virtual void keyPressure(uint8_t channel, uint8_t key, uint8_t value) = 0;
        virtual void allNotesOff() = 0;

        virtual void fillInterleavedStereoBuffer(int16_t *buffer, size_t size) = 0;

        virtual void preloadDls(const od::Guid &dlsGuid) = 0;
        virtual void assignPreset(uint8_t channel, uint32_t bank, uint32_t patch, const od::Guid &dlsGuid) = 0;

    };

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_MIDISYNTH_H_ */
