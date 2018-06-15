/*
 * AnimationDemo.cpp
 *
 *  Created on: 15 Jun 2018
 *      Author: zal
 */

#include "rfl/dragon/AnimationDemo.h"

#include "rfl/Rfl.h"
#include "rfl/PrefetchProbe.h"
#include "LevelObject.h"

namespace odRfl
{

    AnimationDemo::AnimationDemo()
    : mAnimations({})
    , mSwitchPeriodSeconds(10.0)
    , mFirstUpdate(true)
    , mRunningTime(0.0)
    , mCurrentAnimIndex(0)
    {
    }

    void AnimationDemo::probeFields(RflFieldProbe &probe)
    {
        probe("Animation Demo")
                (mAnimations, "Animations")
                (mSwitchPeriodSeconds, "Switch Period (s)");
    }

    void AnimationDemo::loaded(od::Engine &engine, od::LevelObject *obj)
    {
        mAnimations.fetchAssets(obj->getClass()->getModel()->getAssetProvider());

        mAnimationPlayer = new od::SkeletonAnimationPlayer(engine, obj, obj->getSkeletonRoot(), nullptr);

        obj->setEnableRflUpdateHook(true);
    }

    void AnimationDemo::update(od::LevelObject &obj, double simTime, double relTime)
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


    OD_REGISTER_RFL_CLASS(0x000c, "Animation Demo", AnimationDemo);

}
