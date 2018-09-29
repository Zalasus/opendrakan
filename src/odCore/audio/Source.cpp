/*
 * Source.cpp
 *
 *  Created on: 23 Sep 2018
 *      Author: zal
 */

#include <odCore/audio/Source.h>

#include <mutex>

#include <odCore/audio/SoundManager.h>
#include <odCore/db/Sound.h>
#include <odCore/audio/Buffer.h>

namespace od
{

    Source::Source(SoundManager &soundManager)
    : mSoundManager(soundManager)
    , mSourceId(0)
    , mGain(1.0f)
    , mFadingValue(0.0f)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alGenSources(1, &mSourceId);
        SoundManager::doErrorCheck("Could not generate source");
    }

    Source::~Source()
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alSourceStop(mSourceId);
        SoundManager::doErrorCheck("Could not stop source to delete it");

        alSourcei(mSourceId, AL_BUFFER, AL_NONE); // unqueue any buffers
        SoundManager::doErrorCheck("Could not unqueue buffer when deleting source");

        alDeleteSources(1, &mSourceId);
        SoundManager::doErrorCheck("Could not delete source");
    }

    void Source::setPosition(float xPos, float yPos, float zPos)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alSource3f(mSourceId, AL_POSITION, xPos, yPos, zPos);
        SoundManager::doErrorCheck("Could not set source position");
    }

    void Source::setVelocity(float xVel, float yVel, float zVel)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alSource3f(mSourceId, AL_VELOCITY, xVel, yVel, zVel);
        SoundManager::doErrorCheck("Could not set source velocity");
    }

    void Source::setDirection(float xDir, float yDir, float zDir)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alSource3f(mSourceId, AL_DIRECTION, xDir, yDir, zDir);
        SoundManager::doErrorCheck("Could not set source direction");
    }

    void Source::setRelative(bool relative)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        ALint alBoolRelative = relative ? AL_TRUE : AL_FALSE;

        alSourcei(mSourceId, AL_SOURCE_RELATIVE, alBoolRelative);
        SoundManager::doErrorCheck("Could not set source relative state");
    }

    void Source::setPitch(float pitch)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        alSourcef(mSourceId, AL_PITCH, pitch);
        SoundManager::doErrorCheck("Could not set source pitch");
    }

    void Source::setLooping(bool looping)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        ALint alBoolLooping = looping ? AL_TRUE : AL_FALSE;

        alSourcei(mSourceId, AL_LOOPING, alBoolLooping);
        SoundManager::doErrorCheck("Could not set source looping state");
    }

    void Source::setGain(float gain)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        mGain = gain;
        _updateSourceGain_locked();
    }

    void Source::setSound(Sound *s)
    {
        mSound = s;

        if(mSound != nullptr)
        {
            mBuffer = mSound->getOrCreateBuffer(mSoundManager);

            std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

            alSourcei(mSourceId, AL_BUFFER, mBuffer->getId());
            SoundManager::doErrorCheck("Could not attach sound buffer to source");

            mGain = mSound->getLinearGain();
        }
    }

    void Source::play(float fadeInTime)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        if(fadeInTime > 0.0)
        {
            mFadingValue.move(1.0f, fadeInTime);

        }else
        {
            mFadingValue.set(1.0);
        }
        _updateSourceGain_locked();

        alSourcePlay(mSourceId);
        SoundManager::doErrorCheck("Could not play source");
    }

    void Source::stop(float fadeOutTime)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        if(fadeOutTime == 0.0)
        {
            alSourceStop(mSourceId);
            SoundManager::doErrorCheck("Could not stop source");

        }else
        {
            mFadingValue.move(0.0f, fadeOutTime);
            _updateSourceGain_locked();
        }
    }

    void Source::update(float relTime)
    {
        std::lock_guard<std::mutex> lock(mSoundManager.getWorkerMutex());

        if(mFadingValue.update(relTime))
        {
            _updateSourceGain_locked();
        }
    }

    void Source::_updateSourceGain_locked()
    {
        alSourcef(mSourceId, AL_GAIN, mGain*mFadingValue);
        SoundManager::doErrorCheck("Could not set source gain");
    }


}
