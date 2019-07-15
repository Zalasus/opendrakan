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
        Logger::debug() << "Created dummy synth";
    }

    void DummySynth::noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        Logger::debug() << "(note on) n=" << (uint32_t)note << " c=" << (uint32_t)channel << " v=" << (uint32_t)velocity;
    }

    void DummySynth::noteOff(uint8_t channel, uint8_t note)
    {
        Logger::debug() << "(note off) n=" << (uint32_t)note << " c=" << (uint32_t)channel;;
    }

    void DummySynth::pitchBend(uint8_t channel, uint16_t value)
    {
        Logger::debug() << "(pitch bend) channel=" << (uint32_t)channel << " value=" << value;
    }

    void DummySynth::controllerChange(uint8_t channel, uint8_t controller, uint8_t value)
    {
        Logger::debug() << "(controller change) channel=" << (uint32_t)channel << " controller=" << (uint32_t)controller
                << "value=" << (uint32_t)value;
    }

    void DummySynth::channelPressure(uint8_t channel, uint8_t value)
    {
        Logger::debug() << "(channel pressure) channel=" << (uint32_t)channel << " value=" << (uint32_t)value;
    }

    void DummySynth::keyPressure(uint8_t channel, uint8_t key, uint8_t value)
    {
        Logger::debug() << "(channel pressure) channel=" << (uint32_t)channel << " key=" << (uint32_t)key
                 << " value=" << (uint32_t)value;
    }

    void DummySynth::allNotesOff()
    {
         Logger::debug() << "(all notes off)";
    }

    void DummySynth::fillInterleavedStereoBuffer(int16_t *buffer, size_t size)
    {
        for(size_t i = 0; i < size; ++i)
        {
            buffer[i] = 0;
        }
    }

    void DummySynth::preloadDls(const od::Guid &dlsGuid)
    {

    }

    void DummySynth::assignPreset(uint8_t channel, uint32_t bank, uint32_t patch, const od::Guid &dlsGuid)
    {

    }
}


