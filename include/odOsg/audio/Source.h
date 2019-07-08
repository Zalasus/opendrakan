/*
 * Source.h
 *
 *  Created on: Jan 20, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_SOURCE_H_
#define INCLUDE_ODOSG_AUDIO_SOURCE_H_

#include <string>
#include <mutex>

#include <odCore/audio/Source.h>

#include <odCore/anim/Interpolator.h>

#include <AL/al.h>

namespace odDb
{
    class Sound;
}

namespace odOsg
{
    class SoundSystem;
    class Buffer;

    class Source : public odAudio::Source
    {
    public:

        Source(SoundSystem &ss);
        Source(const Source &s) = delete;
        virtual ~Source();

        inline std::mutex &getMutex() { return mMutex; }

        virtual State getState() override;

        virtual void setPosition(const glm::vec3 &p) override;
        virtual void setVelocity(const glm::vec3 &v) override;
        virtual void setDirection(const glm::vec3 &d) override;
        virtual void setRelative(bool relative) override;
        virtual void setPitch(float pitch) override;
        virtual void setLooping(bool looping) override;
        virtual void setGain(float gain) override;

        virtual void setSound(odDb::Sound *s) override;
        virtual void play(float fadeInTime) override;
        virtual void stop(float fadeOutTime) override;

        /// @brief Called from the sound worker thread with the mutex already held.
        virtual void update(float relTime);


    protected:

        SoundSystem &mSoundSystem;
        ALuint mSourceId;
        std::mutex mMutex;


    private:

        template <typename T>
        void _setProperty(ALuint property, const T &value, const std::string &failMsg);

        void _updateSourceGain_locked(); // call only with worker mutex held

        od::RefPtr<odDb::Sound> mCurrentSound;
        od::RefPtr<Buffer> mCurrentBuffer;

        float mSourceGain;
        float mSoundGain;
        odAnim::Interpolated<float> mFadingValue;
    };

}


#endif /* INCLUDE_ODOSG_AUDIO_SOURCE_H_ */
