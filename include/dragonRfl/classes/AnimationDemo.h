/*
 * AnimationDemo.h
 *
 *  Created on: 15 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_
#define INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_

#include <memory>

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

#include <odCore/anim/SkeletonAnimationPlayer.h>

namespace dragonRfl
{

    class DragonRfl;

    class AnimationDemo : public odRfl::RflClass
    {
    public:

        AnimationDemo(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onUpdate(od::LevelObject &obj, float relTime) override;


    protected:

        odRfl::AnimRefArray mAnimations;
        odRfl::Float        mSwitchPeriodSeconds;


    private:

        bool mFirstUpdate;
        float mRunningTime;
        size_t mCurrentAnimIndex;

        std::unique_ptr<odAnim::SkeletonAnimationPlayer> mPlayer;

    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x000c, "Debug", "Animation Demo", dragonRfl::AnimationDemo);

#endif /* INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_ */
