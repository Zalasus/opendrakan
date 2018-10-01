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
    , mFirstSpawn(true)
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
    }

    void SoundEffect::onSpawned(LevelObject &obj)
    {
        obj.setEnableRflUpdateHook(true);

        mSounds.fetchAssets(obj.getClass()->getAssetProvider());
    }

    void SoundEffect::onDespawned(LevelObject &obj)
    {
        mSounds.releaseAssets();
    }

    void SoundEffect::onUpdate(LevelObject &obj, double simTime, double relTime)
    {
        if(mSoundSource == nullptr)
        {
            return;
        }
    }

    void SoundEffect::onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message)
    {
        // seems to trigger on any message
        if(mPlayMode == PlayMode::Triggered || mPlayMode == PlayMode::LoopingAndTriggered)
        {
            // ...stub...
        }
    }


    OD_REGISTER_RFLCLASS(DragonRfl, SoundEffect);

}

