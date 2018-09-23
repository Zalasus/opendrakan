/*
 * SoundManager.h
 *
 *  Created on: 23 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_SOUNDMANAGER_H_
#define INCLUDE_ODCORE_AUDIO_SOUNDMANAGER_H_

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

#ifndef ALTHING_DEFAULT_BUFFER_SIZE
#   define ALTHING_DEFAULT_BUFFER_SIZE 2048
#endif

#ifndef ALTHING_DEFAULT_FREQUENCY
#   define ALTHING_DEFAULT_FREQUENCY 44100
#endif

namespace od
{

    class Source;

    class SoundManager
    {
    public:

        SoundManager(const char *deviceName = NULL, size_t bufferSize = ALTHING_DEFAULT_BUFFER_SIZE, ALCint outputFreq = ALTHING_DEFAULT_FREQUENCY);
        ~SoundManager();

        inline std::mutex &getWorkerMutex() { return mWorkerMutex; }

        void setListenerPosition(float xPos, float yPos, float zPos);
        void setListenerVelocity(float xVel, float yVel, float zVel);

        void addSource(Source *source);
        void removeSource(Source *source);

        /**
         * @brief Fills the passed vector with all available device names.
         *
         * @returns \c true if the list could successfully be filled, \c false if listing devices is not supported.
         */
        static bool listDeviceNames(std::vector<std::string> &deviceList);

        static void doErrorCheck(const std::string &failmsg);


    private:

        void _doWorkerStuff();
        void _doContextErrorCheck(const std::string &failmsg);

        ALCdevice *mDevice;
        ALCcontext *mContext;
        size_t mBufferSize;
        ALCint mOutputFrequency;
        std::thread mWorkerThread;
        std::mutex  mWorkerMutex;
        std::atomic_bool mTerminateWorker;

        std::vector<Source*> mSources;
    };

}

#endif /* INCLUDE_ODCORE_AUDIO_SOUNDMANAGER_H_ */
