/*
 * SoundManager.cpp
 *
 *  Created on: 23 Sep 2018
 *      Author: zal
 */

#include <odCore/audio/SoundManager.h>

#include <algorithm>
#include <cstring>
#include <chrono>

#include <odCore/Exception.h>
#include <odCore/audio/Source.h>
#include <odCore/Logger.h>

// the maximum number of devices we allow to be read from the unsafe device enumeration string
#define OD_OPENAL_DEVICELIST_LENGTH_FAILSAFE 32

// the maximum length one device name may have in the unsafe device enumeration string
#define OD_OPENAL_DEVICESTRING_LENGTH_FAILSAFE 128

namespace od
{

    SoundManager::SoundManager(const char *deviceName)
    : mContext(new SoundContext(deviceName))
    , mTerminateFlag(false)
    {
        mContext->makeCurrent();

        mWorkerThread = std::thread(&SoundManager::_doWorkerStuff, this);
    }

    SoundManager::~SoundManager()
    {
        mTerminateFlag = true;
        mWorkerThread.join();
    }

    void SoundManager::setListenerPosition(float xPos, float yPos, float zPos)
    {
        std::lock_guard<std::mutex> lock(mWorkerMutex);

        alListener3f(AL_POSITION, xPos, yPos, zPos);
        doErrorCheck("Could not set listener position");
    }

    void SoundManager::setListenerOrientation(const osg::Vec3 &at, const osg::Vec3 &up)
    {
        std::lock_guard<std::mutex> lock(mWorkerMutex);

        float atAndUp[6] = { at.x(), at.y(), at.z(),
                             up.x(), up.y(), up.z()  };

        alListenerfv(AL_ORIENTATION, atAndUp);
        doErrorCheck("Could not set listener orientation");
    }

    void SoundManager::setListenerVelocity(float xVel, float yVel, float zVel)
    {
        std::lock_guard<std::mutex> lock(mWorkerMutex);

        alListener3f(AL_VELOCITY, xVel, yVel, zVel);
        doErrorCheck("Could not set listener velocity");
    }

    void SoundManager::setEaxSoundSpace(EaxPreset preset)
    {
        // TODO: here we would translate the EAX preset to the corresponding EFX preset if the EFX extension is available

        throw UnsupportedException("EAX unsupported as of now");
    }

    Source *SoundManager::createSource()
    {
        mSources.push_back(std::unique_ptr<Source>(new Source(*this)));

        return mSources.back().get();
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
        Logger::verbose() << "Started sound worker thread";

        auto lastTime = std::chrono::high_resolution_clock::now();
        while(!mTerminateFlag)
        {
            auto now = std::chrono::high_resolution_clock::now();
            float relTime = std::chrono::duration_cast<std::chrono::duration<float>>(now - lastTime).count();
            lastTime = now;

            try
            {
                for(auto it = mSources.begin(); it != mSources.end(); ++it)
                {
                    (*it)->update(relTime);
                }

            }catch(Exception &e)
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
