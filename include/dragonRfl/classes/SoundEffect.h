/*
 * SoundEffect.h
 *
 *  Created on: Sep 25, 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_SOUNDEFFECT_H_
#define INCLUDE_DRAGONRFL_SOUNDEFFECT_H_

#include <random>

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace odAudio
{
    class Source;
}

namespace dragonRfl
{

    class SoundEffect : public odRfl::LevelObjectClassBase
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

        SoundEffect();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onDespawned() override;
        virtual void onUpdate(float relTime) override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;
        virtual void onTranslated(const glm::vec3 &from, const glm::vec3 &to) override;


    private:

        void _playRandomSound();

        odRfl::SoundRefArray            mSounds;
        odRfl::EnumImpl<PlayMode, 0, 4> mPlayMode;
        odRfl::Float                    mAveragePeriod;
        odRfl::Float                    mPeriodRandomDeviation;
        odRfl::EnumImpl<Location, 0, 3> mLocation;

        od::RefPtr<odAudio::Source> mSoundSource;

        float mTimeUntilNextPlay;

        std::minstd_rand mRandomNumberGenerator;
    };

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::SoundEffect, 0x0006, "Special Effect", "Sound Effect");

#endif /* INCLUDE_DRAGONRFL_SOUNDEFFECT_H_ */
