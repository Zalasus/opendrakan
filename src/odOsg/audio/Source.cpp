/*
 * Source.cpp
 *
 *  Created on: Jan 20, 2019
 *      Author: zal
 */

#include <odOsg/audio/Source.h>

#include <odCore/Exception.h>
#include <odCore/db/Sound.h>
#include <odCore/Downcast.h>

#include <odOsg/audio/SoundSystem.h>
#include <odOsg/audio/Buffer.h>

namespace odOsg
{
    template <>
    void Source::_setProperty<bool>(ALuint property, const bool &value, const std::string &failMsg)
    {
        ALint alBool = value ? AL_TRUE : AL_FALSE;

        alSourcei(mSourceId, property, alBool);
        SoundSystem::doErrorCheck(failMsg);
    }

    template <>
    void Source::_setProperty<float>(ALuint property, const float &value, const std::string &failMsg)
    {
        alSourcef(mSourceId, property, value);
        SoundSystem::doErrorCheck(failMsg);
    }

    template <>
    void Source::_setProperty<glm::vec3>(ALuint property, const glm::vec3 &value, const std::string &failMsg)
    {
        alSource3f(mSourceId, property, value.x, value.y, value.z);
        SoundSystem::doErrorCheck(failMsg);
    }


    Source::Source(SoundSystem &ss)
    : mSoundSystem(ss)
    , mSourceId(0)
    , mSourceGain(1.0)
    , mSoundGain(1.0)
    , mFadingValue(1.0)
    {
        // no need to sync here. there is no way the worker thread could get to us yet
        //std::lock_guard<std::mutex> lock(getMutex());

        alGenSources(1, &mSourceId);
        SoundSystem::doErrorCheck("Could not generate source");
    }

    Source::~Source()
    {
        std::lock_guard<std::mutex> lock(getMutex());

        alSourceStop(mSourceId);
        SoundSystem::doErrorCheck("Could not stop source to delete it");

        alSourcei(mSourceId, AL_BUFFER, AL_NONE); // unqueue any buffers
        SoundSystem::doErrorCheck("Could not unqueue buffer when deleting source");

        alDeleteSources(1, &mSourceId);
        SoundSystem::doErrorCheck("Could not delete source");
    }

    Source::State Source::getState()
    {
        std::lock_guard<std::mutex> lock(getMutex());

        ALint sourceState;
        alGetSourcei(mSourceId, AL_SOURCE_STATE, &sourceState);
        SoundSystem::doErrorCheck("Could not query source state");

        switch(sourceState)
        {
        case AL_INITIAL:
            return State::Initial;

        case AL_PLAYING:
            return State::Playing;

        case AL_PAUSED:
            return State::Paused;

        case AL_STOPPED:
            return State::Stopped;
        }

        throw od::Exception("Got unknown source state");
    }

    void Source::setPosition(const glm::vec3 &p)
    {
        std::lock_guard<std::mutex> lock(getMutex());
        _setProperty(AL_POSITION, p, "Could not set source position");
    }

    void Source::setVelocity(const glm::vec3 &v)
    {
        std::lock_guard<std::mutex> lock(getMutex());
        _setProperty(AL_VELOCITY, v, "Could not set source velocity");
    }

    void Source::setDirection(const glm::vec3 &d)
    {
        std::lock_guard<std::mutex> lock(getMutex());
        _setProperty(AL_DIRECTION, d, "Could not set source direction");
    }

    void Source::setRelative(bool relative)
    {
        std::lock_guard<std::mutex> lock(getMutex());
        _setProperty(AL_SOURCE_RELATIVE, relative, "Could not set source relative state");
    }

    void Source::setPitch(float pitch)
    {
        std::lock_guard<std::mutex> lock(getMutex());
        _setProperty(AL_PITCH, pitch, "Could not set source pitch");
    }

    void Source::setLooping(bool looping)
    {
        std::lock_guard<std::mutex> lock(getMutex());
        _setProperty(AL_LOOPING, looping, "Could not set source looping state");
    }

    void Source::setGain(float gain)
    {
        std::lock_guard<std::mutex> lock(getMutex());
        mSourceGain = gain;
        _updateSourceGain_locked();
    }

    void Source::setSound(std::shared_ptr<odDb::Sound> s)
    {
        std::lock_guard<std::mutex> lock(getMutex());

        mCurrentSound = s;

        if(mCurrentBuffer != nullptr)
        {
            alSourceStop(mSourceId);
            SoundSystem::doErrorCheck("Could not stop source to unqueue buffer");

            alSourcei(mSourceId, AL_BUFFER, AL_NONE); // unqueue any buffers
            SoundSystem::doErrorCheck("Could not unqueue buffer");
        }

        if(mCurrentSound != nullptr)
        {
            auto buffer = mSoundSystem.getOrCreateBuffer(mCurrentSound);
            mCurrentBuffer = od::confident_downcast<Buffer>(buffer);

            alSourcei(mSourceId, AL_BUFFER, mCurrentBuffer->getBufferId());
            SoundSystem::doErrorCheck("Could not attach sound buffer to source");

            // sounds with a lower-than-output sampling rate seem to have a higher volume than indicated by their
            //  volume field. amplitudes seems to be scaled by the resampling factor.
            //  to mimick the way Drakan behaves, we apply this to the overall gain.
            float resamplingGain = mSoundSystem.getContext().getOutputFrequency() / mCurrentSound->getSamplingFrequency();

            mSoundGain = resamplingGain * mCurrentSound->getLinearGain();
            _updateSourceGain_locked();

        }else
        {
            mCurrentBuffer = nullptr;
        }
    }

    void Source::play(float fadeInTime)
    {
        std::lock_guard<std::mutex> lock(getMutex());

        if(fadeInTime > 0.0)
        {
            mFadingValue.move(1.0f, fadeInTime);

        }else
        {
            mFadingValue.set(1.0);
        }
        _updateSourceGain_locked();

        alSourcePlay(mSourceId);
        SoundSystem::doErrorCheck("Could not play source");
    }

    void Source::stop(float fadeOutTime)
    {
        std::lock_guard<std::mutex> lock(getMutex());

        if(fadeOutTime <= 0.0)
        {
            alSourceStop(mSourceId);
            SoundSystem::doErrorCheck("Could not stop source");

        }else
        {
            mFadingValue.move(0.0f, fadeOutTime);
            _updateSourceGain_locked();
        }
    }

    void Source::update(float relTime)
    {
        if(mFadingValue.update(relTime))
        {
            _updateSourceGain_locked();
        }
    }

    void Source::_updateSourceGain_locked()
    {
        alSourcef(mSourceId, AL_GAIN, mSourceGain*mSoundGain*mFadingValue);
        SoundSystem::doErrorCheck("Could not set source gain");
    }

}
