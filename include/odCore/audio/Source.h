/*
 * Source.h
 *
 *  Created on: 23 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_SOURCE_H_
#define INCLUDE_ODCORE_AUDIO_SOURCE_H_

#include <glm/vec3.hpp>

#include <odCore/RefCounted.h>

namespace odDb
{
    class Sound;
}

namespace odAudio
{

    class SoundManager;
    class Buffer;

    class Source : public od::RefCounted
    {
    public:

        enum class State
        {
            Initial,
            Playing,
            Paused,
            Stopped
        };

        virtual ~Source() = default;

        inline bool isPlaying() { return getState() == State::Playing; }

        virtual State getState() = 0;

        virtual void setPosition(const glm::vec3 &p) = 0;
        virtual void setVelocity(const glm::vec3 &v) = 0;
        virtual void setDirection(const glm::vec3 &d) = 0;
        virtual void setRelative(bool relative) = 0;
        virtual void setPitch(float pitch) = 0;
        virtual void setLooping(bool looping) = 0;
        virtual void setGain(float gain) = 0;

        virtual void setSound(odDb::Sound *s) = 0;
        virtual void play(float fadeInTime) = 0;
        virtual void stop(float fadeOutTime) = 0;

        virtual void update(float relTime) = 0;

    };

}

#endif /* INCLUDE_ODCORE_AUDIO_SOURCE_H_ */
