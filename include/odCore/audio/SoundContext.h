/*
 * SoundContext.h
 *
 *  Created on: 24 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_SOUNDCONTEXT_H_
#define INCLUDE_ODCORE_AUDIO_SOUNDCONTEXT_H_

#include <AL/al.h>
#include <AL/alc.h>
#include <string>

#define OD_SOUND_DEFAULT_FREQUENCY 44100

namespace od
{

    /**
     * @brief RAII wrapper for an OpenAL context
     */
    class SoundContext
    {
    public:

        static const ALCint AbsoluteMaximumSources;

        SoundContext(const char *deviceName = NULL, ALCint outputFrequency = OD_SOUND_DEFAULT_FREQUENCY);
        ~SoundContext();

        inline size_t getMaxSourceCount() const { return mMaxSources; }

        void makeCurrent();


    private:

        void _doContextErrorCheck(const std::string &failmsg);

        ALCdevice_struct *mDeviceHandle;
        ALCcontext_struct *mContextHandle;
        ALCint mMaxSources;
    };

}

#endif /* INCLUDE_ODCORE_AUDIO_SOUNDCONTEXT_H_ */
