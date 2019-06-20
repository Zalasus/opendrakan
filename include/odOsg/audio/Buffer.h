/*
 * Buffer.h
 *
 *  Created on: Jan 21, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_BUFFER_H_
#define INCLUDE_ODOSG_AUDIO_BUFFER_H_

#include <odCore/audio/Buffer.h>

#include <AL/al.h>

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
        Buffer(SoundSystem &ss, odDb::Sound *sound);
        virtual ~Buffer();

        inline ALuint getBufferId() const { return mBufferId; }


    private:

        SoundSystem &mSoundSystem;
        ALuint mBufferId;

        od::RefPtr<odDb::Sound> mSound;

    };

}


#endif /* INCLUDE_ODOSG_AUDIO_BUFFER_H_ */
