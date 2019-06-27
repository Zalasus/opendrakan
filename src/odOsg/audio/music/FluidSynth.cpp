/*
 * FluidSynth.cpp
 *
 *  Created on: 25 Jun 2019
 *      Author: zal
 */

#include <odOsg/audio/music/FluidSynth.h>

#include <fluidsynth/settings.h>

#include <odCore/Logger.h>

namespace odOsg
{

    class Settings
    {
    public:

        Settings()
        {
            mInstance = new_fluid_settings();
        }

        ~Settings()
        {
            delete_fluid_settings(mInstance);
        }

        inline fluid_settings_t *getInstance() { return mInstance; }


    private:

        fluid_settings_t *mInstance;

    };



    FluidSynth::FluidSynth()
    : mSynth(nullptr)
    {
        Settings settings;

        mSynth = new_fluid_synth(settings.getInstance());
    }

    FluidSynth::~FluidSynth()
    {
        delete_fluid_synth(mSynth);
    }

    void FluidSynth::noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        Logger::info() << "(note on) n=" << (uint32_t)note << " c=" << (uint32_t)channel << " v=" << (uint32_t)velocity;

        fluid_synth_noteon(mSynth, channel, note, velocity);
    }

    void FluidSynth::noteOff(uint8_t channel, uint8_t note)
    {
        Logger::info() << "(note off) n=" << (uint32_t)note << " c=" << (uint32_t)channel;

        fluid_synth_noteoff(mSynth, channel, note);
    }

}
