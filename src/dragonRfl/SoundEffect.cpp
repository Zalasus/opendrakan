/*
 * SoundEffect.cpp
 *
 *  Created on: Sep 25, 2018
 *      Author: zal
 */

#include <dragonRfl/SoundEffect.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/LevelObject.h>
#include <odCore/audio/Source.h>
#include <odCore/audio/SoundManager.h>
#include <odCore/Level.h>
#include <odCore/Engine.h>

namespace od
{

    SoundEffect::SoundEffect(DragonRfl &rfl)
    : mSounds({})
    , mPlayMode(PlayMode::Looping)
    , mAveragePeriod(6.0f)
    , mPeriodRandomDeviation(1.5f)
    , mLocation(Location::EffectSite)
    , mSoundSource(nullptr)
    , mTimeUntilNextPlay(0.0)
    {
    }

    void SoundEffect::probeFields(FieldProbe &probe)
    {
        probe("Sound Effect")
                (mSounds, "Sounds")
                (mPlayMode, "Play Mode")
                (mAveragePeriod, "Average Period (sec)")
                (mPeriodRandomDeviation, "Period random deviation")
                (mLocation, "Location");
    }

    void SoundEffect::onLoaded(LevelObject &obj)
    {
        obj.setObjectType(LevelObjectType::Detector);

        mSoundSource = obj.getLevel().getEngine().getSoundManager().createSource(); // TODO: integrate into spawn/despawn cycle
        if(mSoundSource == nullptr)
        {
            Logger::warn() << "Sound Effect object failed to create sound source";
        }
    }

    void SoundEffect::onSpawned(LevelObject &obj)
    {
        if(mSoundSource == nullptr)
        {
            return;
        }

        mSounds.fetchAssets(obj.getClass()->getAssetProvider());

        if(mPlayMode == PlayMode::Once || mPlayMode == PlayMode::Intermittent)
        {
            obj.setEnableRflUpdateHook(true);

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
            osg::Vec3 pos = obj.getPosition();
            mSoundSource->setPosition(pos.x(), pos.y(), pos.z());
        }
    }

    void SoundEffect::onDespawned(LevelObject &obj)
    {
        mSoundSource->stop(0.1);

        mSounds.releaseAssets();
    }

    void SoundEffect::onUpdate(LevelObject &obj, double simTime, double relTime)
    {
        if(mSoundSource == nullptr || mSoundSource->isPlaying())
        {
            return;
        }

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

    void SoundEffect::onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message)
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

    void SoundEffect::onMoved(LevelObject &obj)
    {
        if(mLocation != Location::EffectSite)
        {
            return;
        }

        osg::Vec3 pos = obj.getPosition();
        mSoundSource->setPosition(pos.x(), pos.y(), pos.z());
    }

    void SoundEffect::_playRandomSound()
    {
        if(mSoundSource == nullptr || mSounds.getAssetCount() == 0)
        {
            return;
        }

        osg::ref_ptr<Sound> sound;
        if(mSounds.getAssetCount() == 1)
        {
            // don't bother with randomness when only one sound in list
            sound = mSounds.getAsset(0);

        }else
        {
            std::uniform_int_distribution<size_t> uniformDistribution(0, mSounds.getAssetCount() - 1);
            size_t soundIndex = uniformDistribution(mRandomNumberGenerator);

            sound = mSounds.getAsset(soundIndex);
        }

        if(sound == nullptr)
        {
            mSoundSource->setSound(sound);
            mSoundSource->play(0.1);
        }
    }


    OD_REGISTER_RFLCLASS(DragonRfl, SoundEffect);

}

