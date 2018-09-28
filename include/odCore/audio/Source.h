/*
 * Source.h
 *
 *  Created on: 23 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_SOURCE_H_
#define INCLUDE_ODCORE_AUDIO_SOURCE_H_

#include <AL/al.h>
#include <osg/ref_ptr>
#include <memory>

namespace od
{

    class SoundManager;
    class Sound;
    class Buffer;

    class Source
    {
    public:

        friend class SoundManager;

        ~Source();

        inline ALuint getSourceId() const { return mSourceId; }

        void setPosition(float xPos, float yPos, float zPos);
        void setVelocity(float xVel, float yVel, float zVel);
        void setDirection(float xDir, float yDir, float zDir);
        void setRelative(bool relative);
        void setPitch(float pitch);
        void setLooping(bool looping);

        void setSound(Sound *s);
        void play(float fadeInTime = 0.0f);
        void stop(float fadeOutTime = 0.0f);

        void update(double relTime, double simTime);


    protected:

         Source(SoundManager &soundManager);


    private:

        SoundManager &mSoundManager;
        ALuint mSourceId;

        osg::ref_ptr<Sound> mSound;
        std::shared_ptr<Buffer> mBuffer;

        float mFadingTime;
    };

}

#endif /* INCLUDE_ODCORE_AUDIO_SOURCE_H_ */
