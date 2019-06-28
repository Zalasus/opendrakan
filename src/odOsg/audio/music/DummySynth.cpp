/*
 * DummySynth.cpp
 *
 *  Created on: Jun 28, 2019
 *      Author: zal
 */

#include <odOsg/audio/music/DummySynth.h>

#include <odCore/Logger.h>

namespace odOsg
{

    DummySynth::DummySynth()
    {
        Logger::info() << "Created dummy synth";
    }

    void DummySynth::noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        Logger::info() << "(note on) n=" << (uint32_t)note << " c=" << (uint32_t)channel << " v=" << (uint32_t)velocity;
    }

    void DummySynth::noteOff(uint8_t channel, uint8_t note)
    {
        Logger::info() << "(note off) n=" << (uint32_t)note << " c=" << (uint32_t)channel;;
    }

    void DummySynth::fillInterleavedStereoBuffer(int16_t *buffer, size_t size)
    {

    }

}


