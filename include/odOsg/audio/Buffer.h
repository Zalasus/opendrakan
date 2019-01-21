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

namespace odOsg
{

    class SoundSystem;

    class Buffer : public odAudio::Buffer
    {
    public:

        Buffer(SoundSystem &ss);
        virtual ~Buffer();


    private:

        SoundSystem &mSoundSystem;
        ALuint mBufferId;

    };

}


#endif /* INCLUDE_ODOSG_AUDIO_BUFFER_H_ */
