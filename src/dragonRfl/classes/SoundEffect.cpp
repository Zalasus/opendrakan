/*
 * SoundEffect.cpp
 *
 *  Created on: Sep 25, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/SoundEffect.h>

#include <odCore/LevelObject.h>
#include <odCore/Level.h>

#include <odCore/audio/SoundSystem.h>
#include <odCore/audio/Source.h>

#include <dragonRfl/RflDragon.h>

namespace dragonRfl
{

    SoundEffect::SoundEffect()
    : mSounds({})
    , mPlayMode(PlayMode::Looping)
    , mAveragePeriod(6.0f)
    , mPeriodRandomDeviation(1.5f)
    , mLocation(Location::EffectSite)
    , mTimeUntilNextPlay(0.0)
    {
    }

    SoundEffect::~SoundEffect()
    {

    }

    void SoundEffect::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Sound Effect")
                (mSounds, "Sounds")
                (mPlayMode, "Play Mode")
                (mAveragePeriod, "Average Period (sec)")
                (mPeriodRandomDeviation, "Period random deviation")
                (mLocation, "Location");
    }

    void SoundEffect::onLoaded()
    {
        getLevelObject().setObjectType(od::LevelObjectType::Detector);

        odAudio::SoundSystem *soundSystem = nullptr; //getLevelObject().getLevel().getEngine().getSoundSystem();
        if(soundSystem != nullptr)
        {
            mSoundSource = soundSystem->createSource();
        }
    }

    void SoundEffect::onSpawned()
    {
        if(mSoundSource == nullptr)
        {
            return;
        }

        mSounds.fetchAssets(getLevelObject().getClass()->getAssetProvider());

        if(mPlayMode == PlayMode::Once || mPlayMode == PlayMode::Intermittent)
        {
            getLevelObject().setEnableUpdate(true);
            mSoundSource->setLooping(false);

        }else if(mPlayMode == PlayMode::Looping || mPlayMode == PlayMode::LoopingAndTriggered)
        {
            mSoundSource->setLooping(true);
        }

        if(mPlayMode == PlayMode::Once || mPlayMode == PlayMode::Looping)
        {
            _playRandomSound();
        }

        if(mLocation == Location::EffectSite)
        {
            mSoundSource->setPosition(getLevelObject().getPosition());
        }
    }

    void SoundEffect::onDespawned()
    {
        mSounds.releaseAssets();
    }

    void SoundEffect::onUpdate(float relTime)
    {
        if(mSoundSource == nullptr || mSoundSource->isPlaying())
        {
            return;
        }

        if(mPlayMode == PlayMode::Once)
        {
            getLevelObject().requestDestruction();
            return;

        }else if(mPlayMode == PlayMode::Intermittent)
        {
            if(mTimeUntilNextPlay <= 0.0)
            {
                _playRandomSound();

                std::uniform_real_distribution<float> uniformDistribution(-mPeriodRandomDeviation, mPeriodRandomDeviation);
                mTimeUntilNextPlay = mAveragePeriod + uniformDistribution(mRandomNumberGenerator);

            }else
            {
                mTimeUntilNextPlay -= relTime;
            }
        }
    }

    void SoundEffect::onMessageReceived(od::LevelObject &sender, od::Message message)
    {
        if(mSoundSource == nullptr || mSoundSource->isPlaying())
        {
            return;
        }

        // seems to trigger on any message
        if(mPlayMode == PlayMode::Triggered || mPlayMode == PlayMode::LoopingAndTriggered)
        {
            _playRandomSound();
        }
    }

    void SoundEffect::onTranslated(const glm::vec3 &from, const glm::vec3 &to)
    {
        if(mLocation != Location::EffectSite)
        {
            return;
        }

        if(mSoundSource != nullptr)
        {
            mSoundSource->setPosition(to);
        }
    }

    void SoundEffect::_playRandomSound()
    {
        if(mSoundSource == nullptr || mSounds.getAssetCount() == 0)
        {
            return;
        }

        od::RefPtr<odDb::Sound> sound;
        if(mSounds.getAssetCount() == 1)
        {
            // don't bother with randomness when only one sound in list
            sound = mSounds.getAsset(0);

        }else if(mSounds.getAssetCount() > 1)
        {
            std::uniform_int_distribution<size_t> uniformDistribution(0, mSounds.getAssetCount() - 1);
            size_t soundIndex = uniformDistribution(mRandomNumberGenerator);

            sound = mSounds.getAsset(soundIndex);
        }

        if(sound != nullptr)
        {
            mSoundSource->setSound(sound);
            mSoundSource->play(0.1);
            Logger::verbose() << "Sound Source now playing '" << sound->getName() << "'";
        }
    }

}

