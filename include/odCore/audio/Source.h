/*
 * Source.h
 *
 *  Created on: 23 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_SOURCE_H_
#define INCLUDE_ODCORE_AUDIO_SOURCE_H_

#include <AL/al.h>

namespace od
{

    class SoundManager;
    class Sound;

    class Source
    {
    public:

        Source(SoundManager &soundManager);
        ~Source();

        inline ALuint getSourceId() const { return mSourceId; }

        void setPosition(float xPos, float yPos, float zPos);
        void setVelocity(float xVel, float yVel, float zVel);
        void setDirection(float xDir, float yDir, float zDir);
        void setRelative(bool relative);

        void setSound(Sound *s);
        void play();
        void stop();

    private:

        SoundManager &mSoundManager;
        ALuint mSourceId;
    };

}

#endif /* INCLUDE_ODCORE_AUDIO_SOURCE_H_ */
