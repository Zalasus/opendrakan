/*
 * Source.h
 *
 *  Created on: Jan 20, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_SOURCE_H_
#define INCLUDE_ODOSG_AUDIO_SOURCE_H_

#include <string>

#include <odCore/audio/Source.h>

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

        virtual void update(float relTime) override;


    private:

        template <typename T>
        void _setProperty(ALuint property, const T &value, const std::string &failMsg);

        SoundSystem &mSoundSystem;
        ALuint mSourceId;

        od::RefPtr<odDb::Sound> mCurrentSound;
        od::RefPtr<Buffer> mCurrentBuffer;

        float mGain; // soon to be replaced by interpolating float
    };

}


#endif /* INCLUDE_ODOSG_AUDIO_SOURCE_H_ */
