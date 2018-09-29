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
#include <deque>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

#include <odCore/audio/EaxPresets.h>
#include <odCore/audio/SoundContext.h>

namespace od
{

    class Source;
    class Sound;

    class SoundManager
    {
    public:

        SoundManager(const char *deviceName = NULL);
        ~SoundManager();

        inline std::mutex &getWorkerMutex() { return mWorkerMutex; }
        inline std::shared_ptr<SoundContext> &getContext() { return mContext; }

        void setListenerPosition(float xPos, float yPos, float zPos);
        void setListenerVelocity(float xVel, float yVel, float zVel);

        void setEaxSoundSpace(EaxPreset preset);

        Source *createSource();

        /**
         * @brief Fills the passed vector with all available device names.
         *
         * @returns \c true if the list could successfully be filled, \c false if listing devices is not supported.
         */
        static bool listDeviceNames(std::vector<std::string> &deviceList);

        static void doErrorCheck(const std::string &failmsg);


    private:

        void _doWorkerStuff();

        std::shared_ptr<SoundContext> mContext;

        std::thread mWorkerThread;
        std::mutex  mWorkerMutex;
        std::atomic_bool mTerminateFlag;

        std::deque<std::unique_ptr<Source>> mSources;
    };

}

#endif /* INCLUDE_ODCORE_AUDIO_SOUNDMANAGER_H_ */
