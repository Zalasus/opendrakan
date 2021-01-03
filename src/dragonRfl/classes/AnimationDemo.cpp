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

#include <odCore/render/Renderer.h>

#include <odCore/Client.h>
#include <odCore/LevelObject.h>

namespace dragonRfl
{

    AnimationDemoFields::AnimationDemoFields()
    : mAnimations({})
    , mSwitchPeriodSeconds(10.0)
    {
    }

    void AnimationDemoFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Animation Demo")
                (mAnimations, "Animations")
                (mSwitchPeriodSeconds, "Switch Period (s)");
    }


    AnimationDemo::AnimationDemo()
    : mFirstUpdate(true)
    , mRunningTime(0.0)
    , mCurrentAnimIndex(0)
    {
    }

    odRfl::Spawnable *AnimationDemo::getSpawnable()
    {
        return this;
    }

    odRfl::FieldBundle &AnimationDemo::getFieldBundle()
    {
        return *this;
    }

    void AnimationDemo::onSpawned()
    {
        od::LevelObject &obj = getLevelObject();

        if(!obj.getClass()->hasModel())
        {
            return;
        }

        mAnimations.fetchAssets(obj.getClass()->getModel()->getAssetProvider());

        mRenderHandle = getClient().getRenderer().createHandleFromObject(obj);

        odAnim::Skeleton *skeleton = obj.getOrCreateSkeleton();
        if(skeleton == nullptr)
        {
            Logger::warn() << "Animation Demo class used on object without skeleton";
            return;
        }

        mAnimPlayer = std::make_unique<odAnim::SkeletonAnimationPlayer>(skeleton);

        obj.setEnableUpdate(true);
    }

    void AnimationDemo::onUpdate(float relTime)
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
            if(currentAnimation == nullptr || mAnimPlayer == nullptr)
            {
                return;
            }

            auto playbackType = currentAnimation->isLooping() ? odAnim::PlaybackType::Looping : odAnim::PlaybackType::Normal;

            mAnimPlayer->playAnimation(currentAnimation, playbackType, 1.0f);

            Logger::verbose() << "Animation Demo now playing '" << currentAnimation->getName() << "'";
        }

        if(mAnimPlayer != nullptr)
        {
            bool skeletonChanged = mAnimPlayer->update(relTime);
            if(skeletonChanged && mRenderHandle != nullptr)
            {
                getLevelObject().getOrCreateSkeleton()->flatten(mRenderHandle->getRig());
            }
        }
    }

}
