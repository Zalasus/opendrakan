/*
 * SoundEffect.cpp
 *
 *  Created on: Sep 25, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/SoundEffect.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/Engine.h>

namespace dragonRfl
{

    SoundEffect::SoundEffect(DragonRfl &rfl)
    : mSounds({})
    , mPlayMode(PlayMode::Looping)
    , mAveragePeriod(6.0f)
    , mPeriodRandomDeviation(1.5f)
    , mLocation(Location::EffectSite)
    , mTimeUntilNextPlay(0.0)
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

    void SoundEffect::onLoaded(od::LevelObject &obj)
    {
        obj.setObjectType(od::LevelObjectType::Detector);
    }

    void SoundEffect::onSpawned(od::LevelObject &obj)
    {
        mSounds.fetchAssets(obj.getClass()->getAssetProvider());

        if(mPlayMode == PlayMode::Once || mPlayMode == PlayMode::Intermittent)
        {
            obj.setEnableRflUpdateHook(true);

        }else if(mPlayMode == PlayMode::Looping || mPlayMode == PlayMode::LoopingAndTriggered)
        {
        }

        if(mPlayMode == PlayMode::Once || mPlayMode == PlayMode::Looping)
        {
            _playRandomSound();
        }

        if(mLocation == Location::EffectSite)
        {
        }
    }

    void SoundEffect::onDespawned(od::LevelObject &obj)
    {
        mSounds.releaseAssets();
    }

    void SoundEffect::onUpdate(od::LevelObject &obj, float relTime)
    {
        if(mPlayMode == PlayMode::Once)
        {
            obj.requestDestruction();
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

    void SoundEffect::onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message)
    {
        // seems to trigger on any message
        if(mPlayMode == PlayMode::Triggered || mPlayMode == PlayMode::LoopingAndTriggered)
        {
            _playRandomSound();
        }
    }

    void SoundEffect::onMoved(od::LevelObject &obj)
    {
        if(mLocation != Location::EffectSite)
        {
            return;
        }
    }

    void SoundEffect::_playRandomSound()
    {
        odDb::AssetPtr<odDb::Sound> sound;
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
            Logger::verbose() << "Sound Source now playing '" << sound->getName() << "'";
        }
    }


    OD_REGISTER_RFLCLASS(DragonRfl, SoundEffect);

}

