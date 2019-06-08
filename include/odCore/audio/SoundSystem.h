/*
 * SoundSystem.h
 *
 *  Created on: Jan 20, 2019
 *      Author: zal
 */

#ifndef SRC_ODCORE_AUDIO_SOUNDSYSTEM_H_
#define SRC_ODCORE_AUDIO_SOUNDSYSTEM_H_

#include <glm/vec3.hpp>

#include <odCore/RefCounted.h>

#include <odCore/audio/EaxPresets.h>

namespace odDb
{
    class Sound;
}

namespace odAudio
{

    class Source;
    class Buffer;

    typedef uint32_t MusicId;

    /**
     * @brief Abstract interface for a sound system implementation.
     */
    class SoundSystem
    {
    public:

        virtual ~SoundSystem() = default;

        virtual void setListenerPosition(const glm::vec3 &pos) = 0;
        virtual void setListenerOrientation(const glm::vec3 &at, const glm::vec3 &up) = 0;
        virtual void setListenerVelocity(const glm::vec3 &v) = 0;

        virtual od::RefPtr<Source> createSource() = 0;
        virtual od::RefPtr<Buffer> createBuffer(odDb::Sound *sound) = 0;

        virtual void setEaxPreset(EaxPreset preset) = 0;

        virtual void playMusic(MusicId musicId) = 0;
        virtual void stopMusic() = 0;

    };

}


#endif /* SRC_ODCORE_AUDIO_SOUNDSYSTEM_H_ */
