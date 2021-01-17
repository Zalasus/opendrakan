
#include <odCore/audio/SoundSystem.h>

#include <odCore/Panic.h>

#include <odCore/db/Sound.h>

namespace odAudio
{

    std::shared_ptr<Buffer> SoundSystem::getOrCreateBuffer(std::shared_ptr<odDb::Sound> sound)
    {
        OD_CHECK_ARG_NONNULL(sound);

        if(!sound->getCachedSoundBuffer().expired())
        {
            auto cachedBuffer = sound->getCachedSoundBuffer().lock();
            if(cachedBuffer != nullptr)
            {
                return cachedBuffer;
            }
        }

        auto newBuffer = this->createBuffer(sound);

        sound->getCachedSoundBuffer() = newBuffer;

        return newBuffer;
    }

}
