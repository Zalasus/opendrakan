/*
 * SkeletonAnimationPlayer.cpp
 *
 *  Created on: 4 Dec 2018
 *      Author: zal
 */

#include <odCore/anim/SkeletonAnimationPlayer.h>

#include <odCore/Logger.h>
#include <odCore/Exception.h>

#include <odCore/anim/MotionAccumulator.h>

namespace odAnim
{

    static void _dquatToTranslationAndRotation(const glm::dualquat &dq, glm::vec3 &trans, glm::quat &rot)
    {
        // see paper "A Beginners Guide to Dual-Quaternions" by Ben Kenwright
        rot = dq.real;

        glm::quat transQuat = dq.dual * glm::conjugate(dq.real);
        trans = 2.0f * glm::vec3(transQuat.x, transQuat.y, transQuat.z);
    }


    BoneAnimator::BoneAnimator(Skeleton::Bone *bone)
    : mBone(bone)
    , mPlaying(false)
    , mCurrentTime(0)
    , mAccumulator(nullptr)
    {
        if(mBone == nullptr)
        {
            throw od::Exception("Created BoneAnimator with bone = nullptr");
        }
    }

    void BoneAnimator::setAnimation(odDb::Animation *animation)
    {
        mPlaying = false;

        mCurrentAnimation = animation;
        if(mCurrentAnimation != nullptr)
        {
            mKeyframesStartEnd = mCurrentAnimation->getKeyframesForNode(mBone->getJointIndex());
        }
    }

    void BoneAnimator::play()
    {
        if(mCurrentAnimation == nullptr)
        {
            return;
        }

        mBone->move(glm::mat4(mKeyframesStartEnd.first->xform));

        if(mKeyframesStartEnd.first+1 == mKeyframesStartEnd.second)
        {
            // single KF animation. we are done here
            mPlaying = false;

        }else
        {
            mCurrentKeyframe = mKeyframesStartEnd.first;
            mLeftTransform = glm::dualquat(mCurrentKeyframe->xform);
            mRightTransform = glm::dualquat((mCurrentKeyframe+1)->xform);
            mCurrentTime = 0;
            mPlaying = true;
        }
    }

    void BoneAnimator::update(float relTime)
    {
        if(!mPlaying || mCurrentAnimation == nullptr || mCurrentKeyframe+1 >= mKeyframesStartEnd.second)
        {
            return;
        }

        mCurrentTime += relTime;

        bool continous = true;
        glm::dualquat prevRightXform;

        if(mCurrentTime >= (mCurrentKeyframe+1)->time)
        {
            // advance until we are at valid position again
            while((mCurrentKeyframe+1) < mKeyframesStartEnd.second)
            {
                if(mCurrentKeyframe->time <= mCurrentTime && (mCurrentKeyframe+1)->time > mCurrentTime)
                {
                    break;
                }

                ++mCurrentKeyframe;
            }

            // did we advance past the last frame? if yes, stop animation or loop
            if(mCurrentKeyframe+1 >= mKeyframesStartEnd.second)
            {
                if(mCurrentAnimation->isLooping())
                {
                    mCurrentTime = mCurrentTime - mCurrentKeyframe->time;
                    mCurrentKeyframe = mKeyframesStartEnd.first;

                    continous = false;
                    prevRightXform = mRightTransform;

                }else
                {
                    mPlaying = false;
                    return;
                }
            }

            mLeftTransform = glm::dualquat(mCurrentKeyframe->xform);
            mRightTransform = glm::dualquat((mCurrentKeyframe+1)->xform);
        }

        // delta==0 -> exactly at current frame, delta==1 -> exactly at next frame
        float delta = (mCurrentTime - mCurrentKeyframe->time)/((mCurrentKeyframe+1)->time - mCurrentKeyframe->time);

        glm::dualquat interpolatedTransform = glm::lerp(mLeftTransform, mRightTransform, delta);

        if(mAccumulator == nullptr)
        {
            glm::mat4 asMat(glm::mat3x4_cast(interpolatedTransform));
            mBone->move(asMat);

        }else
        {
            glm::vec3 prevOffset;
            glm::quat prevRotation;
            _dquatToTranslationAndRotation(mPreviousTransform, prevOffset, prevRotation);

            glm::vec3 currentOffset;
            glm::quat currentRotation;
            _dquatToTranslationAndRotation(interpolatedTransform, currentOffset, currentRotation);

            glm::vec3 relativeOffset = currentOffset - prevOffset;
            glm::quat relativeRotation = currentRotation * glm::conjugate(prevRotation);

            // if the current animation step looped back in time, we need to factor out the offset
            //  between the last keyframe and the first (see diagram I drew which I keep in a drawer somewhere)
            if(!continous)
            {
                glm::vec3 leftKfOffset;
                glm::quat leftKfRotation;
                _dquatToTranslationAndRotation(mLeftTransform, leftKfOffset, leftKfRotation);

                glm::vec3 rightKfOffset;
                glm::quat rightKfRotation;
                _dquatToTranslationAndRotation(prevRightXform, rightKfOffset, rightKfRotation);

                relativeOffset += rightKfOffset - leftKfOffset;
                relativeRotation *= rightKfRotation * glm::conjugate(leftKfRotation);
            }

            mAccumulator->pushMotionState(relativeOffset, relativeRotation, relTime);

            mBone->moveToBindPose();
        }

        mPreviousTransform = interpolatedTransform;
    }


    SkeletonAnimationPlayer::SkeletonAnimationPlayer(odRender::ObjectNode *objectNode, Skeleton *skeleton)
    : mObjectNode(objectNode)
    , mSkeleton(skeleton)
    , mRig(nullptr)
    {
        if(mSkeleton == nullptr)
        {
            throw od::Exception("Tried to create SkeletonAnimationPlayer with null skeleton");
        }

        mBoneAnimators.reserve(mSkeleton->getBoneCount());
        for(size_t i = 0; i < mSkeleton->getBoneCount(); ++i)
        {
            mBoneAnimators.push_back(BoneAnimator(mSkeleton->getBoneByJointIndex(i)));
        }

        if(mObjectNode != nullptr)
        {
            mObjectNode->addFrameListener(this);

            mRig = mObjectNode->getRig();
            if(mRig == nullptr)
            {
                throw od::Exception("Failed to get Rig from object node");
            }

        }else
        {
            // TODO: Hook us into another update callback here
        }
    }

    SkeletonAnimationPlayer::~SkeletonAnimationPlayer()
    {
        mObjectNode->removeFrameListener(this);
    }

    void SkeletonAnimationPlayer::playAnimation(odDb::Animation *anim)
    {
        for(auto it = mBoneAnimators.begin(); it != mBoneAnimators.end(); ++it)
        {
            it->setAnimation(anim);
            it->play();
        }
    }

    void SkeletonAnimationPlayer::playAnimation(odDb::Animation *anim, int32_t jointIndex)
    {
        throw od::UnsupportedException("Partial skeleton animation not implmented yet");
    }

    void SkeletonAnimationPlayer::setRootNodeAccumulator(MotionAccumulator *accu, int32_t rootNodeIndex)
    {
        if(rootNodeIndex < 0 || rootNodeIndex >= (int32_t)mBoneAnimators.size())
        {
            throw od::InvalidArgumentException("Root node index out of bounds");
        }

        BoneAnimator &animator = mBoneAnimators[rootNodeIndex];
        if(!animator.getBone()->isRoot())
        {
            throw od::InvalidArgumentException("Selected bone for accumulation is not a root bone");
        }

        animator.setAccumulator(accu);
    }

    void SkeletonAnimationPlayer::onFrameUpdate(double simTime, double relTime, uint32_t frameNumber)
    {
        for(auto it = mBoneAnimators.begin(); it != mBoneAnimators.end(); ++it)
        {
            it->update((float)relTime);
        }

        if(mRig != nullptr)
        {
            // TODO: only flatten if any updates to the skeleton's pose happened
            mSkeleton->flatten(mRig);
        }
    }

}
