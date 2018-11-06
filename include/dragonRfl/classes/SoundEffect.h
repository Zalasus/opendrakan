/*
 * SoundEffect.h
 *
 *  Created on: Sep 25, 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_SOUNDEFFECT_H_
#define INCLUDE_DRAGONRFL_SOUNDEFFECT_H_

#include <random>

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace dragonRfl
{
    class DragonRfl;

    class SoundEffect : public odRfl::RflClass
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

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onLoaded(od::LevelObject &obj) override;
        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onDespawned(od::LevelObject &obj) override;
        virtual void onUpdate(od::LevelObject &obj, float relTime) override;
        virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message) override;
        virtual void onMoved(od::LevelObject &obj) override;


    private:

        void _playRandomSound();

        odRfl::SoundRefArray            mSounds;
        odRfl::EnumImpl<PlayMode, 0, 4> mPlayMode;
        odRfl::Float                    mAveragePeriod;
        odRfl::Float                    mPeriodRandomDeviation;
        odRfl::EnumImpl<Location, 0, 3> mLocation;

        float mTimeUntilNextPlay;

        std::minstd_rand mRandomNumberGenerator;
    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0006, "Special Effect", "Sound Effect", dragonRfl::SoundEffect);

#endif /* INCLUDE_DRAGONRFL_SOUNDEFFECT_H_ */
