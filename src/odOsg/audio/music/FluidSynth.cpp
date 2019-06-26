/*
 * FluidSynth.cpp
 *
 *  Created on: 25 Jun 2019
 *      Author: zal
 */

#include <odOsg/audio/music/FluidSynth.h>

#include <odCore/Logger.h>

namespace odOsg
{

    FluidSynth::FluidSynth()
    {

    }

    void FluidSynth::noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        Logger::info() << "(note on) n=" << (uint32_t)note << " c=" << (uint32_t)channel << " v=" << (uint32_t)velocity;
    }

    void FluidSynth::noteOff(uint8_t channel, uint8_t note)
    {
        Logger::info() << "(note off) n=" << (uint32_t)note << " c=" << (uint32_t)channel;
    }

}
