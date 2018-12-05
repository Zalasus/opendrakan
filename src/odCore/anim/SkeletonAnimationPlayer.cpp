/*
 * SkeletonAnimationPlayer.cpp
 *
 *  Created on: 4 Dec 2018
 *      Author: zal
 */

#include <odCore/anim/SkeletonAnimationPlayer.h>

#include <odCore/Logger.h>

namespace odAnim
{

    BoneAnimator::BoneAnimator(Skeleton::Bone *bone)
    : mBone(bone)
    {
    }

    void BoneAnimator::setAnimation(odDb::Animation *animation)
    {
        mCurrentAnimation = animation;
        if(mCurrentAnimation != nullptr)
        {
            mKeyframes = mCurrentAnimation->getKeyframesForNode(mBone->getJointIndex());
        }
    }

    void BoneAnimator::update(float relTime)
    {
        Logger::info() << "Bone animator update " << relTime;
    }


    SkeletonAnimationPlayer::SkeletonAnimationPlayer(odRender::ObjectNode *objectNode, Skeleton *skeleton)
    : mObjectNode(objectNode)
    , mSkeleton(skeleton)
    {
        mBoneAnimators.reserve(mSkeleton->getBoneCount());
        for(size_t i = 0; i < mSkeleton->getBoneCount(); ++i)
        {
            mBoneAnimators.push_back(BoneAnimator(mSkeleton->getBoneByJointIndex(i)));
        }

        mObjectNode->addFrameListener(this);
    }

    SkeletonAnimationPlayer::~SkeletonAnimationPlayer()
    {
        mObjectNode->removeFrameListener(this);
    }

    void SkeletonAnimationPlayer::playAnimation(odDb::Animation *anim)
    {

    }

    void SkeletonAnimationPlayer::playAnimation(odDb::Animation *anim, int32_t jointIndex)
    {

    }

    void SkeletonAnimationPlayer::onFrameUpdate(double simTime, double relTime, uint32_t frameNumber)
    {
        for(auto it = mBoneAnimators.begin(); it != mBoneAnimators.end(); ++it)
        {
            it->update((float)relTime);
        }
    }

}
