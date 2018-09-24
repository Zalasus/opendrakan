/*
 * SoundContext.cpp
 *
 *  Created on: 24 Sep 2018
 *      Author: zal
 */

#include <odCore/audio/SoundContext.h>

#include <odCore/Exception.h>

namespace od
{

    const ALCint SoundContext::AbsoluteMaximumSources = 128;

    SoundContext::SoundContext(const char *deviceName, ALCint outputFrequency)
    : mDeviceHandle(nullptr)
    , mContextHandle(nullptr)
    {
        mDeviceHandle = alcOpenDevice(deviceName);
        _doContextErrorCheck("Could not open device");

        const ALCint contextAttributes[] =
            {
                ALC_FREQUENCY, outputFrequency,
                0
                // TODO: add more context attributes
            };
        mContextHandle = alcCreateContext(mDeviceHandle, &contextAttributes[0]);
        _doContextErrorCheck("Could not create context");

        ALCint maxMonoSources = 0;
        ALCint maxStereoSources = 0;
        alcGetIntegerv(mDeviceHandle, ALC_MONO_SOURCES, 1, &maxMonoSources);
        _doContextErrorCheck("Could not get mono source count");
        alcGetIntegerv(mDeviceHandle, ALC_STEREO_SOURCES, 1, &maxStereoSources);
        _doContextErrorCheck("Could not get stereo source count");
        mMaxSources = std::min(maxMonoSources+maxStereoSources, AbsoluteMaximumSources);
    }

    SoundContext::~SoundContext()
    {
        alcDestroyContext(mContextHandle);
        alcCloseDevice(mDeviceHandle);
    }

    void SoundContext::makeCurrent()
    {
        alcMakeContextCurrent(mContextHandle);
        _doContextErrorCheck("Could not make context current");
    }

    void SoundContext::_doContextErrorCheck(const std::string &failmsg)
    {
        std::string alErrorMsg;
        ALenum error = alcGetError(mDeviceHandle);
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
