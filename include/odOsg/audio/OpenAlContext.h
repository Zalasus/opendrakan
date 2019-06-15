/*
 * OpenAlContext.h
 *
 *  Created on: 24 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_OPENALCONTEXT_H_
#define INCLUDE_ODCORE_AUDIO_OPENALCONTEXT_H_

#include <AL/al.h>
#include <AL/alc.h>
#include <string>

#include <odOsg/Constants.h>

namespace odOsg
{

    /**
     * @brief RAII wrapper for an OpenAL context
     */
    class OpenAlContext
    {
    public:

        static const ALCint AbsoluteMaximumSources;

        OpenAlContext(const char *deviceName = NULL, ALCint outputFrequency = Constants::AUDIO_SAMPLE_RATE);
        OpenAlContext(const OpenAlContext &c) = delete;
        ~OpenAlContext();

        inline size_t getMaxSourceCount() const { return mMaxSources; }
        inline uint32_t getOutputFrequency() const { return mOutputFrequency; }

        void makeCurrent();


    private:

        void _doContextErrorCheck(const std::string &failmsg);

        uint32_t mOutputFrequency;
        ALCdevice *mDeviceHandle;
        ALCcontext *mContextHandle;
        ALCint mMaxSources;
    };

}

#endif /* INCLUDE_ODCORE_AUDIO_OPENALCONTEXT_H_ */
