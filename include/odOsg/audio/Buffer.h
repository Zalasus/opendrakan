/*
 * Buffer.h
 *
 *  Created on: Jan 21, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_BUFFER_H_
#define INCLUDE_ODOSG_AUDIO_BUFFER_H_

#include <memory>
#include <AL/al.h>

#include <odCore/audio/Buffer.h>

namespace odDb
{
    class Sound;
}

namespace odOsg
{

    class SoundSystem;

    class Buffer : public odAudio::Buffer
    {
    public:

        Buffer(SoundSystem &ss);
        Buffer(SoundSystem &ss, std::shared_ptr<odDb::Sound> sound);
        virtual ~Buffer();

        inline ALuint getBufferId() const { return mBufferId; }


    private:

        SoundSystem &mSoundSystem;
        ALuint mBufferId;

        std::shared_ptr<odDb::Sound> mSound;

    };

}


#endif /* INCLUDE_ODOSG_AUDIO_BUFFER_H_ */
