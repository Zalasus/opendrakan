/*
 * Buffer.cpp
 *
 *  Created on: Jan 21, 2019
 *      Author: zal
 */

#include <odOsg/audio/Buffer.h>

#include <odCore/Exception.h>
#include <odCore/db/Sound.h>

#include <odOsg/audio/SoundSystem.h>

namespace odOsg
{

    Buffer::Buffer(SoundSystem &ss)
    : mSoundSystem(ss)
    , mBufferId(0)
    {
        alGenBuffers(1, &mBufferId);
        SoundSystem::doErrorCheck("Could not generate buffer");
    }

    Buffer::Buffer(SoundSystem &soundSystem, std::shared_ptr<odDb::Sound> sound)
    : Buffer(soundSystem)
    {
        mSound = sound;

        uint32_t bitsPerChannel = mSound->getBitsPerChannel();
        uint32_t channelCount = mSound->getChannelCount();
        ALenum format;
        if(bitsPerChannel == 8 && channelCount == 1)
        {
            format = AL_FORMAT_MONO8;

        }else if(bitsPerChannel == 8 && channelCount == 2)
        {
            format = AL_FORMAT_STEREO8;

        }else if(bitsPerChannel ==  16 && channelCount == 1)
        {
            format = AL_FORMAT_MONO16;

        }else if(bitsPerChannel ==  16 && channelCount == 2)
        {
            format = AL_FORMAT_STEREO16;

        }else
        {
            Logger::error() << "Sound '" << sound->getName() << "' has unsupported format (bits/channel=" << bitsPerChannel
                    << ", channels=" << channelCount << ")";
            throw od::UnsupportedException("Sound has unsupported format");
        }

        const auto &data = mSound->getDataBuffer();
        alBufferData(mBufferId, format, data.data(), data.size(), mSound->getSamplingFrequency());
        SoundSystem::doErrorCheck("Could not fill buffer with data");
    }

    Buffer::~Buffer()
    {
        alDeleteBuffers(1, &mBufferId);
        SoundSystem::doErrorCheck("Could not delete buffer");
    }

}
