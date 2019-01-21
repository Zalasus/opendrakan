/*
 * OpenAlContext.cpp
 *
 *  Created on: 24 Sep 2018
 *      Author: zal
 */

#include <odOsg/audio/OpenAlContext.h>

#include <odCore/Exception.h>

namespace odOsg
{

    const ALCint OpenAlContext::AbsoluteMaximumSources = 128;

    OpenAlContext::OpenAlContext(const char *deviceName, ALCint outputFrequency)
    : mOutputFrequency(outputFrequency)
    , mDeviceHandle(nullptr)
    , mContextHandle(nullptr)
    {
        mDeviceHandle = alcOpenDevice(deviceName);
        _doContextErrorCheck("Could not open device");
        if(mDeviceHandle == nullptr)
        {
            throw od::Exception("Could not open device (alcOpenDevice return nullptr)");
        }

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

    OpenAlContext::~OpenAlContext()
    {
        alcDestroyContext(mContextHandle);

        bool success = alcCloseDevice(mDeviceHandle);
        if(!success)
        {
            Logger::warn() << "Failed to close AL device";
        }
    }

    void OpenAlContext::makeCurrent()
    {
        alcMakeContextCurrent(mContextHandle);
        _doContextErrorCheck("Could not make context current");
    }

    void OpenAlContext::_doContextErrorCheck(const std::string &failmsg)
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

        throw od::Exception(failmsg + alErrorMsg);
    }

}
