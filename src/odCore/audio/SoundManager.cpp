/*
 * SoundManager.cpp
 *
 *  Created on: 23 Sep 2018
 *      Author: zal
 */

#include <odCore/audio/SoundManager.h>

#include <algorithm>
#include <cstring>

#include <odCore/Exception.h>
#include <odCore/audio/Source.h>
#include <odCore/Logger.h>

// the maximum number of devices we allow to be read from the unsafe device enumeration string
#define OD_OPENAL_DEVICELIST_LENGTH_FAILSAFE 32

// the maximum length one device name may have in the unsafe device enumeration string
#define OD_OPENAL_DEVICESTRING_LENGTH_FAILSAFE 128

namespace od
{

    SoundManager::SoundManager(const char *deviceName, size_t bufferSize, ALCint outputFreq)
    : mDevice(nullptr)
    , mContext(nullptr)
    , mBufferSize(bufferSize)
    , mOutputFrequency(outputFreq)
    , mWorkerThread()
    , mTerminateWorker(false)
    {
        mDevice = alcOpenDevice(deviceName);
        _doContextErrorCheck("Could not open device");

        const ALCint contextAttributes[] =
            {
                ALC_FREQUENCY, mOutputFrequency,
                0
                // TODO: add more context attributes
            };
        mContext = alcCreateContext(mDevice, &contextAttributes[0]);
        _doContextErrorCheck("Could not create context");

        alcMakeContextCurrent(mContext);
        _doContextErrorCheck("Could not make context current");

        //std::function<void(void)> workerFunction = std::bind(&SoundManager::_doWorkerStuff, this);
        //mWorkerThread = std::thread(workerFunction); // TODO: catch exception if thread can't be created
    }

    SoundManager::~SoundManager()
    {
        //mTerminateWorker = true;
        //mWorkerThread.join();

        for(Source *s : mSources)
        {
            delete s;
        }

        alcDestroyContext(mContext);
        alcCloseDevice(mDevice);
    }

    void SoundManager::setListenerPosition(float xPos, float yPos, float zPos)
    {
        std::lock_guard<std::mutex> lock(mWorkerMutex);

        alListener3f(AL_POSITION, xPos, yPos, zPos);
        doErrorCheck("Could not set listener position");
    }

    void SoundManager::setListenerVelocity(float xVel, float yVel, float zVel)
    {
        std::lock_guard<std::mutex> lock(mWorkerMutex);

        alListener3f(AL_VELOCITY, xVel, yVel, zVel);
        doErrorCheck("Could not set listener velocity");
    }

    void SoundManager::addSource(Source *source)
    {
        if(source == nullptr)
        {
            return;
        }

        mSources.push_back(source);
    }

    void SoundManager::removeSource(Source *source)
    {
        auto it = std::find(mSources.begin(), mSources.end(), source);
        if(it != mSources.end())
        {
            mSources.erase(it);
        }
    }

    bool SoundManager::listDeviceNames(std::vector<std::string> &deviceList)
    {
#if ALC_ENUMERATE_ALL_EXT != 1
        return false;
#else
        deviceList.clear();

        const char *strList = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
        if(strList == nullptr)
        {
            throw Exception("Could not access device list");
        }

        // the returned string consists of concatenated c-strings, terminated with an empty string
        //  WARNING: there is absolutely no indication of how long this string actually might be. if OpenAL messes up, the
        //  following code might cause access violations or at least produce garbage until our failsafe kicks in
        size_t stringLength = strlen(strList);
        while(stringLength > 0
                && deviceList.size() <= OD_OPENAL_DEVICELIST_LENGTH_FAILSAFE
                && stringLength <= OD_OPENAL_DEVICESTRING_LENGTH_FAILSAFE)
        {
            deviceList.push_back(std::string(strList));

            strList += stringLength + 1;
            stringLength = strlen(strList);
        }

        if(stringLength != 0 &&
            (   deviceList.size() > OD_OPENAL_DEVICELIST_LENGTH_FAILSAFE
             || stringLength > OD_OPENAL_DEVICESTRING_LENGTH_FAILSAFE))
        {
            Logger::warn() << "Had to terminate device enumaration because the number or length of device names exceeded a sane amount";
        }

        return true;
#endif
    }

    void SoundManager::doErrorCheck(const std::string &failmsg)
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

        throw Exception(failmsg + alErrorMsg);
    }

    void SoundManager::_doWorkerStuff()
    {
        while(!mTerminateWorker)
        {
            // scoping this so mutex gets unlocked when we sleep
            {
                std::lock_guard<std::mutex> lock(mWorkerMutex);

                for(Source *s : mSources)
                {
                    ALint queueSize;
                    alGetSourcei(s->getSourceId(), AL_BUFFERS_QUEUED, &queueSize);
                    doErrorCheck("Error while polling source queue size");

                    ALint buffersProcessed;
                    alGetSourcei(s->getSourceId(), AL_BUFFERS_PROCESSED, &buffersProcessed);
                    doErrorCheck("Error while polling source's processed buffers");
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void SoundManager::_doContextErrorCheck(const std::string &failmsg)
    {
        std::string alErrorMsg;
        ALenum error = alcGetError(mDevice);
        switch(error)
        {
        case ALC_NO_ERROR:
            return;

        case ALC_INVALID_DEVICE:
            alErrorMsg = " (invalid device)";
            break;

        case ALC_INVALID_CONTEXT:
            alErrorMsg = " (invalid context)";
            break;

        case ALC_INVALID_ENUM:
            alErrorMsg = " (invalid enum)";
            break;

        case ALC_INVALID_VALUE:
            alErrorMsg = " (invalid value)";
            break;

        case ALC_OUT_OF_MEMORY:
            alErrorMsg = " (out of memory)";
            break;

        default:
            alErrorMsg = " (unknown error)";
            break;
        }

        throw Exception(failmsg + alErrorMsg);
    }

}
