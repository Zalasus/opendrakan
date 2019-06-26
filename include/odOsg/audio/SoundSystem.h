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
#include <memory>

#include <odCore/WeakRefPtr.h>

#include <odCore/audio/SoundSystem.h>

#include <odOsg/audio/OpenAlContext.h>

namespace odDb
{
    class MusicContainer;
}

namespace odAudio
{
    class SegmentPlayer;
}

namespace odOsg
{
    class Source;

    class SoundSystem : public odAudio::SoundSystem
    {
    public:

        SoundSystem();
        virtual ~SoundSystem();

        inline OpenAlContext &getContext() { return mContext; }

        virtual void setListenerPosition(const glm::vec3 &pos) override;
        virtual void setListenerOrientation(const glm::vec3 &at, const glm::vec3 &up) override;
        virtual void setListenerVelocity(const glm::vec3 &v) override;

        virtual od::RefPtr<odAudio::Source> createSource() override;
        virtual od::RefPtr<odAudio::Buffer> createBuffer(odDb::Sound *sound) override;

        virtual void setEaxPreset(odAudio::EaxPreset preset) override;

        virtual void loadMusicContainer(const od::FilePath &rrcPath) override;
        virtual void playMusic(odAudio::MusicId musicId) override;
        virtual void stopMusic() override;


        static void doErrorCheck(const std::string &failmsg);


    private:

        void _doWorkerStuff();
        void _doMusicStuff();

        OpenAlContext mContext;

        std::thread mWorkerThread;
        std::atomic_bool mTerminateFlag;

        std::vector<od::WeakObserverRefPtr<Source>> mSources;

        std::unique_ptr<odDb::MusicContainer> mMusicContainer;
        std::thread mMusicThread;
        std::atomic_bool mShouldTerminateMusicThread;
        std::mutex mMusicWorkerMutex;
        odAudio::SegmentPlayer *mSegmentPlayer;

    };

}


#endif /* INCLUDE_ODOSG_AUDIO_SOUNDSYSTEM_H_ */
