/*
 * SoundEffect.h
 *
 *  Created on: Sep 25, 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_SOUNDEFFECT_H_
#define INCLUDE_DRAGONRFL_SOUNDEFFECT_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace od
{
    class DragonRfl;

    class SoundEffect : public RflClass
    {
    public:

        enum class PlayMode
        {
            Once,
            Looping,
            Intermittent,
            Triggered,
            LoopingAndTriggered
        };

        enum class Location
        {
            EffectSite,
            Player,
            Camera
        };

        SoundEffect(DragonRfl &rfl);

        virtual void probeFields(FieldProbe &probe) override;

        virtual void onLoaded(LevelObject &obj) override;
        virtual void onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message) override;


    private:

        RflSoundRefArray            mSounds;
        RflEnumImpl<PlayMode, 0, 4> mPlayMode;
        RflFloat                    mAveragePeriod;
        RflFloat                    mPeriodRandomDeviation;
        RflEnumImpl<Location, 0, 3> mLocation;

    };


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x0006, "Special Effect", "Sound Effect", SoundEffect);

}


#endif /* INCLUDE_DRAGONRFL_SOUNDEFFECT_H_ */
