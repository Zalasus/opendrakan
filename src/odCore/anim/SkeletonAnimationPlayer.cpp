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

    static glm::vec3 _translationFromDquat(const glm::dualquat &dq)
    {
        // see paper "A Beginners Guide to Dual-Quaternions" by Ben Kenwright
        glm::quat transQuat = dq.dual * glm::conjugate(dq.real);
        return 2.0f * glm::vec3(transQuat.x, transQuat.y, transQuat.z);
    }


    BoneAnimator::BoneAnimator(Skeleton::Bone *bone)
    : mBone(bone)
    , mPlaying(false)
    , mAccumulator(nullptr)
    , mBoneAccumulationFactors(0.0)
    , mObjectAccumulationFactors(0.0)
    , mNonContinous(false)
    {
        if(mBone == nullptr)
        {
            throw od::Exception("Created BoneAnimator with bone = nullptr");
        }
    }

    void BoneAnimator::setAnimation(odDb::Animation *animation)
    {
        mCurrentAnimation = animation;
        if(mCurrentAnimation != nullptr)
        {
            mAnimStartEnd = mCurrentAnimation->getKeyframesForNode(mBone->getJointIndex());
        }
    }

    void BoneAnimator::play()
    {
        if(mCurrentAnimation == nullptr)
        {
            return;
        }

        mPlaying = true;
    }

    void BoneAnimator::setAccumulationModes(const AxesModes &modes)
    {
        for(size_t i = 0; i < 3; ++i)
        {
            switch(modes[i])
            {
            case AccumulationMode::Bone:
                mObjectAccumulationFactors[i] = 0.0;
                mBoneAccumulationFactors[i] = 1.0;
                break;

            case AccumulationMode::Accumulate:
                mObjectAccumulationFactors[i] = 1.0;
                mBoneAccumulationFactors[i] = 0.0;
                break;

            case AccumulationMode::Ignore:
            default:
                mObjectAccumulationFactors[i] = 0.0;
                mBoneAccumulationFactors[i] = 0.0;
                break;
            }
        }
    }

    bool BoneAnimator::update(float relTime, float animTime, bool nonContinous)
    {
        if(!mPlaying || mCurrentAnimation == nullptr)
        {
            return false;
        }

        odDb::Animation::KfIteratorPair currentKeyframes = mCurrentAnimation->getLeftAndRightKeyframe(mBone->getJointIndex(), animTime);

        // only update decompositions when necessary
        if(mLastKeyframes != currentKeyframes)
        {
            mLeftTransform = glm::dualquat(currentKeyframes.first->xform);
            mRightTransform = glm::dualquat(currentKeyframes.second->xform);
            mLastKeyframes = currentKeyframes;
        }

        // delta==0 -> exactly at current frame, delta==1 -> exactly at next frame
        float delta = (animTime - currentKeyframes.first->time)/(currentKeyframes.second->time - currentKeyframes.first->time);

        glm::dualquat interpolatedTransform = glm::lerp(mLeftTransform, mRightTransform, glm::clamp(delta, 0.0f, 1.0f));

        if(mAccumulator == nullptr)
        {
            glm::mat4 asMat(glm::mat3x4_cast(interpolatedTransform));
            mBone->move(asMat);

        }else
        {
            glm::vec3 prevOffset = _translationFromDquat(mLastAppliedTransform);
            glm::vec3 currentOffset = _translationFromDquat(interpolatedTransform);

            glm::vec3 relativeOffset = currentOffset - prevOffset;

            // if the current animation step looped back in time, we need to factor out the offset
            //  between the last keyframe and the first (see diagram I drew which I keep in a drawer somewhere)
            if(mNonContinous)
            {
                glm::vec3 leftKfOffset = _translationFromDquat(mLeftTransform);
                glm::vec3 rightKfOffset = _translationFromDquat(prevRightXform);

                relativeOffset += rightKfOffset - leftKfOffset;
            }

            mAccumulator->moveRelative(relativeOffset * mObjectAccumulationFactors, relTime);

            glm::mat4 boneMatrix = glm::mat4_cast(interpolatedTransform.real); // real part represents rotation
            glm::vec3 boneTranslation = currentOffset * mBoneAccumulationFactors;
            boneMatrix[3] = glm::vec4(boneTranslation, 1.0);
            mBone->move(glm::transpose(boneMatrix));
        }

        mLastAppliedTransform = interpolatedTransform;

        return true;
    }


    SkeletonAnimationPlayer::SkeletonAnimationPlayer(odRender::ObjectNode *objectNode, Skeleton *skeleton)
    : mObjectNode(objectNode)
    , mSkeleton(skeleton)
    , mRig(nullptr)
    , mPlaying(false)
    , mAnimTime(0.0)
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

    void SkeletonAnimationPlayer::playAnimation(odDb::Animation *anim, bool looping)
    {
        for(auto it = mBoneAnimators.begin(); it != mBoneAnimators.end(); ++it)
        {
            it->setAnimation(anim);
            it->play(looping);
        }

        mPlaying = true;
    }

    void SkeletonAnimationPlayer::playAnimation(odDb::Animation *anim, int32_t jointIndex, bool looping)
    {
        throw od::UnsupportedException("Partial skeleton animation not implemented yet");
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

    void SkeletonAnimationPlayer::setRootNodeAccumulationModes(const AxesModes &modes, int32_t rootNodeIndex)
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

        animator.setAccumulationModes(modes);
    }

    void SkeletonAnimationPlayer::onFrameUpdate(double simTime, double relTime, uint32_t frameNumber)
    {
        if(!mPlaying)
        {
            return;
        }

        mAnimTime += relTime;

        bool stillPlaying = true;
        bool needsFlattening = false;
        for(auto it = mBoneAnimators.begin(); it != mBoneAnimators.end(); ++it)
        {
            needsFlattening |= it->update((float)relTime, mAnimTime);
            stillPlaying |= it->isPlaying();
        }

        mPlaying = stillPlaying; // TODO: invoke callback when finished?

        // note: even if we are no longer playing by now, we still might need to flatten the last frame.
        //   thus, we don't check for the playing flag here
        if(mRig != nullptr && needsFlattening)
        {
            mSkeleton->flatten(mRig);
        }
    }

}
