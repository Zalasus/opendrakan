/*
 * SoundEffect.cpp
 *
 *  Created on: Sep 25, 2018
 *      Author: zal
 */

#include <dragonRfl/SoundEffect.h>

#include <dragonRfl/RflDragon.h>

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


    OD_REGISTER_RFLCLASS(DragonRfl, SoundEffect);

}

