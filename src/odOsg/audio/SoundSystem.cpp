/*
 * SoundSystem.cpp
 *
 *  Created on: Jan 20, 2019
 *      Author: zal
 */

#include <odOsg/audio/SoundSystem.h>

#include <AL/al.h>

#include <odCore/Exception.h>

#include <odOsg/audio/Source.h>
#include <odOsg/audio/Buffer.h>

namespace odOsg
{

    SoundSystem::SoundSystem()
    : mContext() // only support default device for now
    , mTerminateFlag(false)
    {
        mContext.makeCurrent();

        mWorkerThread = std::thread(&SoundSystem::_doWorkerStuff, this);
    }

    SoundSystem::~SoundSystem()
    {
        mTerminateFlag = true;
        mWorkerThread.join();
    }

    void SoundSystem::setListenerPosition(const glm::vec3 &pos)
    {
        std::lock_guard<std::mutex> lock(mWorkerMutex);

        alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
        doErrorCheck("Could not set listener position");
    }

    void SoundSystem::setListenerOrientation(const glm::vec3 &at, const glm::vec3 &up)
    {
        std::lock_guard<std::mutex> lock(mWorkerMutex);

        float atAndUp[6] = { at.x, at.y, at.z,
                             up.x, up.y, up.z  };

        alListenerfv(AL_ORIENTATION, atAndUp);
        doErrorCheck("Could not set listener orientation");
    }

    void SoundSystem::setListenerVelocity(const glm::vec3 &v)
    {
        std::lock_guard<std::mutex> lock(mWorkerMutex);

        alListener3f(AL_VELOCITY, v.x, v.y, v.z);
        doErrorCheck("Could not set listener velocity");
    }

    od::RefPtr<odAudio::Source> SoundSystem::createSource()
    {
        auto source = od::make_refd<Source>(*this);

        mSources.push_back(source);

        return source.get();
    }

    od::RefPtr<odAudio::Buffer> SoundSystem::createBuffer()
    {
        auto buffer = od::make_refd<Buffer>(*this);

        return buffer.get();
    }

    void SoundSystem::setEaxPreset(odAudio::EaxPreset preset)
    {
    }

    void SoundSystem::doErrorCheck(const std::string &failmsg)
    {
        std::string alErrorMsg;
        ALenum error = alGetError();
        switch(error)
        {
        case AL_NO_ERROR:
            return;

        case AL_INVALID_NAME:
            alErrorMsg = " (invalid name)";
            break;

        case AL_INVALID_ENUM:
            alErrorMsg = " (invalid enum)";
            break;

        case AL_INVALID_OPERATION:
            alErrorMsg = " (invalid operation)";
            break;

        case AL_INVALID_VALUE:
            alErrorMsg = " (invalid value)";
            break;

        case AL_OUT_OF_MEMORY:
            alErrorMsg = " (out of memory)";
            break;

        default:
            alErrorMsg = " (unknown error)";
            break;
        }

        throw od::Exception(failmsg + alErrorMsg);
    }

    void SoundSystem::_doWorkerStuff()
    {
        Logger::verbose() << "Started sound worker thread";

        auto lastTime = std::chrono::high_resolution_clock::now();
        while(!mTerminateFlag)
        {
            auto now = std::chrono::high_resolution_clock::now();
            float relTime = std::chrono::duration_cast<std::chrono::duration<float>>(now - lastTime).count();
            lastTime = now;

            try
            {
                for(auto &source : mSources)
                {
                    source->update(relTime);
                }

            }catch(od::Exception &e)
            {
                Logger::error() << "Error in sound worker thread: " << e.what();
                Logger::error() << "Terminating sound worker thread...";
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        Logger::verbose() << "Terminated sound worker thread";
    }

}


