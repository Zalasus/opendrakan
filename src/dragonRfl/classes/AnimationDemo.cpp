/*
 * AnimationDemo.cpp
 *
 *  Created on: 15 Jun 2018
 *      Author: zal
 */

#include <dragonRfl/classes/AnimationDemo.h>

#include <dragonRfl/RflDragon.h>

#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/LevelObject.h>
#include <odCore/Level.h>

namespace dragonRfl
{

    AnimationDemo::AnimationDemo(DragonRfl &rfl)
    : mAnimations({})
    , mSwitchPeriodSeconds(10.0)
    , mFirstUpdate(true)
    , mRunningTime(0.0)
    , mCurrentAnimIndex(0)
    {
    }

    void AnimationDemo::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Animation Demo")
                (mAnimations, "Animations")
                (mSwitchPeriodSeconds, "Switch Period (s)");
    }

    void AnimationDemo::onSpawned(od::LevelObject &obj)
    {
        mAnimations.fetchAssets(obj.getClass()->getModel()->getAssetProvider());

        odRender::ObjectNode *objNode = obj.getRenderNode();
        if(objNode == nullptr)
        {
            return; // no need to warn. we are probably running on a server
        }

        odAnim::Skeleton *skeleton = obj.getOrCreateSkeleton();
        if(skeleton == nullptr)
        {
            Logger::warn() << "Animation Demo class used on object without skeleton";
            return;
        }

        mPlayer = std::make_unique<odAnim::SkeletonAnimationPlayer>(objNode, skeleton);

        obj.setEnableRflUpdateHook(true);
    }

    void AnimationDemo::onUpdate(od::LevelObject &obj, float relTime)
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

            odDb::Animation *currentAnimation = mAnimations.getAsset(mCurrentAnimIndex);
            if(currentAnimation == nullptr || mPlayer == nullptr)
            {
                return;
            }

            mPlayer->playAnimation(currentAnimation, currentAnimation->isLooping());

            Logger::verbose() << "Animation Demo now playing '" << currentAnimation->getName() << "'";
        }
    }


    OD_REGISTER_RFLCLASS(DragonRfl, AnimationDemo);

}
