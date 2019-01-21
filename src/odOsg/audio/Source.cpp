/*
 * Source.cpp
 *
 *  Created on: Jan 20, 2019
 *      Author: zal
 */

#include <odOsg/audio/Source.h>

#include <odCore/Exception.h>

#include <odOsg/audio/SoundSystem.h>

namespace odOsg
{
    template <>
    void Source::_setProperty<bool>(ALuint property, const bool &value, const std::string &failMsg)
    {
        std::lock_guard<std::mutex> lock(mSoundSystem.getWorkerMutex());

        ALint alBool = value ? AL_TRUE : AL_FALSE;

        alSourcei(mSourceId, property, alBool);
        SoundSystem::doErrorCheck(failMsg);
    }

    template <>
    void Source::_setProperty<float>(ALuint property, const float &value, const std::string &failMsg)
    {
        std::lock_guard<std::mutex> lock(mSoundSystem.getWorkerMutex());
        alSourcef(mSourceId, property, value);
        SoundSystem::doErrorCheck(failMsg);
    }

    template <>
    void Source::_setProperty<glm::vec3>(ALuint property, const glm::vec3 &value, const std::string &failMsg)
    {
        std::lock_guard<std::mutex> lock(mSoundSystem.getWorkerMutex());
        alSource3f(mSourceId, property, value.x, value.y, value.z);
        SoundSystem::doErrorCheck(failMsg);
    }


    Source::Source(SoundSystem &ss)
    : mSoundSystem(ss)
    , mSourceId(0)
    {
        std::lock_guard<std::mutex> lock(mSoundSystem.getWorkerMutex());

        alGenSources(1, &mSourceId);
        SoundSystem::doErrorCheck("Could not generate source");
    }

    Source::~Source()
    {
        alDeleteSources(1, &mSourceId);
        SoundSystem::doErrorCheck("Could not delete source");
    }

    Source::State Source::getState()
    {
        std::lock_guard<std::mutex> lock(mSoundSystem.getWorkerMutex());

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
        _setProperty(AL_POSITION, p, "Could not set source position");
    }

    void Source::setVelocity(const glm::vec3 &v)
    {
        _setProperty(AL_VELOCITY, v, "Could not set source velocity");
    }

    void Source::setDirection(const glm::vec3 &d)
    {
        _setProperty(AL_DIRECTION, d, "Could not set source direction");
    }

    void Source::setRelative(bool relative)
    {
        _setProperty(AL_SOURCE_RELATIVE, relative, "Could not set source relative state");
    }

    void Source::setPitch(float pitch)
    {
        _setProperty(AL_PITCH, pitch, "Could not set source pitch");
    }

    void Source::setLooping(bool looping)
    {
        _setProperty(AL_LOOPING, looping, "Could not set source looping state");
    }

    void Source::setGain(float gain)
    {
    }

    void Source::setSound(odDb::Sound *s)
    {
    }

    void Source::play(float fadeInTime)
    {
    }

    void Source::stop(float fadeOutTime)
    {
    }

    void Source::update(float relTime)
    {
    }

}
