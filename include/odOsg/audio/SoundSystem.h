/*
 * SoundSystem.h
 *
 *  Created on: Jan 20, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_SOUNDSYSTEM_H_
#define INCLUDE_ODOSG_AUDIO_SOUNDSYSTEM_H_

#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <vector>

#include <odCore/WeakRefPtr.h>

#include <odCore/audio/SoundSystem.h>

#include <odOsg/audio/OpenAlContext.h>

namespace odOsg
{

    class Source;

    class SoundSystem : public odAudio::SoundSystem
    {
    public:

        SoundSystem();
        virtual ~SoundSystem();

        inline std::mutex &getWorkerMutex() { return mWorkerMutex; }
        inline OpenAlContext &getContext() { return mContext; }

        virtual void setListenerPosition(const glm::vec3 &pos) override;
        virtual void setListenerOrientation(const glm::vec3 &at, const glm::vec3 &up) override;
        virtual void setListenerVelocity(const glm::vec3 &v) override;

        virtual od::RefPtr<odAudio::Source> createSource() override;
        virtual od::RefPtr<odAudio::Buffer> createBuffer(odDb::Sound *sound) override;

        virtual void setEaxPreset(odAudio::EaxPreset preset) override;


        static void doErrorCheck(const std::string &failmsg);


    private:

        void _doWorkerStuff();

        OpenAlContext mContext;

        std::thread mWorkerThread;
        std::mutex  mWorkerMutex;
        std::atomic_bool mTerminateFlag;

        std::vector<od::WeakRefPtr<Source>> mSources;
    };

}


#endif /* INCLUDE_ODOSG_AUDIO_SOUNDSYSTEM_H_ */
