/*
 * AnimationDemo.cpp
 *
 *  Created on: 15 Jun 2018
 *      Author: zal
 */

#include <dragonRfl/AnimationDemo.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/PrefetchProbe.h>
#include <odCore/LevelObject.h>
#include <odCore/Level.h>

namespace od
{

    AnimationDemo::AnimationDemo()
    : mAnimations({})
    , mSwitchPeriodSeconds(10.0)
    , mFirstUpdate(true)
    , mRunningTime(0.0)
    , mCurrentAnimIndex(0)
    {
    }

    void AnimationDemo::probeFields(FieldProbe &probe)
    {
        probe("Animation Demo")
                (mAnimations, "Animations")
                (mSwitchPeriodSeconds, "Switch Period (s)");
    }

    void AnimationDemo::onLoaded(LevelObject &obj)
    {
        mAnimations.fetchAssets(obj.getClass()->getModel()->getAssetProvider());

        mAnimationPlayer = new od::SkeletonAnimationPlayer(obj.getLevel().getEngine(), &obj, obj.getSkeletonRoot(), nullptr);

        obj.setEnableRflUpdateHook(true);
    }

    void AnimationDemo::onUpdate(LevelObject &obj, double simTime, double relTime)
    {
        if(mAnimations.getAssetCount() == 0)
        {
            return;
        }

        mRunningTime += relTime;

        if(mFirstUpdate || mRunningTime >= mSwitchPeriodSeconds)
        {
            mFirstUpdate = false;
            mRunningTime = 0.0;
            ++mCurrentAnimIndex;

            if(mCurrentAnimIndex >= mAnimations.getAssetCount())
            {
                mCurrentAnimIndex = 0;
            }

            mAnimationPlayer->stop();
            mAnimationPlayer->setAnimation(mAnimations.getAsset(mCurrentAnimIndex), 0.0);
            mAnimationPlayer->play(true);
        }
    }


    OD_REGISTER_RFL_CLASS(DragonRfl, 0x000c, "Debug", "Animation Demo", AnimationDemo);

}
