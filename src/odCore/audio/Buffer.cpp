/*
 * Buffer.cpp
 *
 *  Created on: 27 Sep 2018
 *      Author: zal
 */

#include <odCore/audio/Buffer.h>

#include <mutex>

#include <odCore/audio/SoundManager.h>
#include <odCore/Exception.h>

namespace od
{

    Buffer::Buffer(SoundManager &soundManager)
    : mSoundManager(soundManager)
    , mBufferId(0)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alGenBuffers(1, &mBufferId);
        SoundManager::doErrorCheck("Could not generate buffer");
    }

    Buffer::~Buffer()
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alDeleteBuffers(1, &mBufferId);
        // FIXME: for some reason this is broken. pls fix
        //SoundManager::doErrorCheck("Could not delete buffer");
    }

    Buffer::Format Buffer::getFormatFor(size_t nBits, size_t nChannels) const
    {
        if(nChannels == 1)
        {
            if(nBits == 8)
            {
                return Format::Mono8Bit;

            }else if(nBits == 16)
            {
                return Format::Mono16Bit;
            }

        }else if(nChannels == 2)
        {
            if(nBits == 8)
            {
                return Format::Stereo8Bit;

            }else if(nBits == 16)
            {
                return Format::Stereo16Bit;
            }
        }

        throw UnsupportedException("Unsupported channel count & bit depth combination");
    }

    void Buffer::setData(uint8_t *data, size_t size, Format format, size_t frequency)
    {
        ALenum alFormat = _toAlFormat(format);

        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alBufferData(mBufferId, alFormat, data, size, frequency);
        SoundManager::doErrorCheck("Could not generate buffer");
    }

    ALenum Buffer::_toAlFormat(Format format)
    {
        switch(format)
        {
        case Format::Mono8Bit:
            return AL_FORMAT_MONO8;

        case Format::Mono16Bit:
            return AL_FORMAT_MONO16;

        case Format::Stereo8Bit:
            return AL_FORMAT_STEREO8;

        case Format::Stereo16Bit:
            return AL_FORMAT_STEREO16;
        }

        throw InvalidArgumentException("Unrecognized format argument");
    }

}

