/*
 * SoundEffect.cpp
 *
 *  Created on: Sep 25, 2018
 *      Author: zal
 */

#include <dragonRfl/SoundEffect.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/LevelObject.h>

namespace od
{

    SoundEffect::SoundEffect(DragonRfl &rfl)
    : mSounds({})
    , mPlayMode(PlayMode::Looping)
    , mAveragePeriod(6.0f)
    , mPeriodRandomDeviation(1.5f)
    , mLocation(Location::EffectSite)
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

