/*
 * StupidSineSynth.h
 *
 *  Created on: Jun 28, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_MUSIC_STUPIDSINESYNTH_H_
#define INCLUDE_ODOSG_AUDIO_MUSIC_STUPIDSINESYNTH_H_

#include <vector>

#include <odCore/audio/music/MidiSynth.h>

namespace odOsg
{

    /**
     * Simple MIDI synth for debugging. Synthesizes notes from a single channel as pure sine tones without any attack curves etc.
     *
     * This sounds horrible. Do not use in production code!
     */
    class StupidSineSynth : public odAudio::MidiSynth
    {
    public:

        StupidSineSynth(uint8_t channel);

        virtual void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
        virtual void noteOff(uint8_t channel, uint8_t note) override;
        virtual void pitchBend(uint8_t channel, uint16_t value) override;
        virtual void controllerChange(uint8_t channel, uint8_t controller, uint8_t value) override;
        virtual void channelPressure(uint8_t channel, uint8_t value) override;
        virtual void keyPressure(uint8_t channel, uint8_t key, uint8_t value) override;
        virtual void allNotesOff() override;

        virtual void fillInterleavedStereoBuffer(int16_t *buffer, size_t size) override;

        void setChannel(uint8_t channel);


    private:

        uint8_t mChannel;
        std::vector<bool> mNotes;
        std::vector<double> mFrequencies;
        double mT;

    };

}


#endif /* INCLUDE_ODOSG_AUDIO_MUSIC_STUPIDSINESYNTH_H_ */
