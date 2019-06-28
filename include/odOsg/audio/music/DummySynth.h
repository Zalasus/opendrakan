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

    class DummySynth : public odAudio::MidiSynth
    {
    public:

        DummySynth();

        virtual void noteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
        virtual void noteOff(uint8_t channel, uint8_t note) override;

        virtual void fillInterleavedStereoBuffer(int16_t *buffer, size_t size) override;

    };

}


#endif /* INCLUDE_ODOSG_AUDIO_MUSIC_DUMMYSYNTH_H_ */
