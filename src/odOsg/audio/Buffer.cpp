/*
 * Buffer.cpp
 *
 *  Created on: Jan 21, 2019
 *      Author: zal
 */

#include <odOsg/audio/Buffer.h>

#include <odOsg/audio/SoundSystem.h>

namespace odOsg
{

    Buffer::Buffer(SoundSystem &soundSystem)
    : mSoundSystem(soundSystem)
    , mBufferId(0)
    {
        alGenBuffers(1, &mBufferId);
        SoundSystem::doErrorCheck("Could not generate buffer");
    }

    Buffer::~Buffer()
    {
        alDeleteBuffers(1, &mBufferId);
        SoundSystem::doErrorCheck("Could not delete buffer");
    }

}
