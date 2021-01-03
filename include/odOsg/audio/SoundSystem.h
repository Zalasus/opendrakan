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

#include <odCore/audio/SoundSystem.h>

#include <odOsg/audio/OpenAlContext.h>

namespace odDb
{
    class MusicContainer;
}

namespace odAudio
{
    class SegmentPlayer;
    class MidiSynth;
}

namespace odOsg
{
    class Source;
    class StreamingSource;

    class SoundSystem : public odAudio::SoundSystem
    {
    public:

        SoundSystem();
        virtual ~SoundSystem();

        inline OpenAlContext &getContext() { return mContext; }

        virtual void setListenerPosition(const glm::vec3 &pos) override;
        virtual void setListenerOrientation(const glm::vec3 &at, const glm::vec3 &up) override;
        virtual void setListenerVelocity(const glm::vec3 &v) override;

        virtual std::shared_ptr<odAudio::Source> createSource() override;
        virtual std::shared_ptr<odAudio::Buffer> createBuffer(std::shared_ptr<odDb::Sound> sound) override;

        virtual void setEaxPreset(odAudio::EaxPreset preset) override;

        virtual void loadMusicContainer(const od::FilePath &rrcPath) override;
        virtual void playMusic(odAudio::MusicId musicId) override;
        virtual void stopMusic() override;


        static void doErrorCheck(const std::string &failmsg);


    private:

        void _doWorkerStuff();

        OpenAlContext mContext;

        std::thread mWorkerThread;
        std::atomic_bool mTerminateFlag;
        std::mutex mWorkerMutex;

        std::vector<std::weak_ptr<Source>> mSources;

        std::unique_ptr<odDb::MusicContainer> mMusicContainer;

        std::shared_ptr<odOsg::StreamingSource> mMusicSource;
        std::unique_ptr<odAudio::MidiSynth> mSynth;
        std::unique_ptr<odAudio::SegmentPlayer> mSegmentPlayer;
    };

}


#endif /* INCLUDE_ODOSG_AUDIO_SOUNDSYSTEM_H_ */
