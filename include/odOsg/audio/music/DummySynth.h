/*
 * DummySynth.h
 *
 *  Created on: Jun 28, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_MUSIC_DUMMYSYNTH_H_
#define INCLUDE_ODOSG_AUDIO_MUSIC_DUMMYSYNTH_H_

#include <odCore/audio/music/MidiSynth.h>

namespace odOsg
{

    /**
     * Dummy implementation of the synth interface. Logs events, but produces only silence otherwise.
     */
    class DummySynth : public odAudio::MidiSynth
    {
    public:

        DummySynth();

        virtual void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
        virtual void noteOff(uint8_t channel, uint8_t note) override;
        virtual void pitchBend(uint8_t channel, uint16_t value) override;
        virtual void controllerChange(uint8_t channel, uint8_t controller, uint8_t value) override;
        virtual void channelPressure(uint8_t channel, uint8_t value) override;
        virtual void keyPressure(uint8_t channel, uint8_t key, uint8_t value) override;
        virtual void allNotesOff() override;

        virtual void fillInterleavedStereoBuffer(int16_t *buffer, size_t size) override;

        virtual void preloadDls(const od::Guid &dlsGuid) override;
        virtual void assignPreset(uint8_t channel, uint32_t bank, uint32_t patch, const od::Guid &dlsGuid) override;
    };

}


#endif /* INCLUDE_ODOSG_AUDIO_MUSIC_DUMMYSYNTH_H_ */
