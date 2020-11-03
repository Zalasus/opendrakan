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

    static glm::vec3 _translationFrom3x4(const glm::mat3x4 &mat)
    {
        return glm::vec3(mat[0].w, mat[1].w, mat[2].w);
    }


    BoneAnimator::BoneAnimator(Skeleton::Bone &bone)
    : mBone(bone)
    , mPlaybackType(PlaybackType::NORMAL)
    , mSpeedMultiplier(1.0f)
    , mPlaying(false)
    , mAnimTime(0.0f)
    , mAccumulator(nullptr)
    , mBoneAccumulationFactors(0.0)
    , mObjectAccumulationFactors(0.0)
    , mUseInterpolation(false)
    {
    }

    void BoneAnimator::playAnimation(std::shared_ptr<odDb::Animation> animation, PlaybackType type, float speedMultiplier)
    {
        if(animation == nullptr)
        {
            mPlaying = false;
            mCurrentAnimation = nullptr;
            return;
        }

        mCurrentAnimation = animation;
        mPlaybackType = type;
        mSpeedMultiplier = speedMultiplier;
        mPlaying = true;

        odDb::Animation::KfIteratorPair newStartEnd = animation->getKeyframesForNode(mBone.getJointIndex());
        size_t frameCount = newStartEnd.second - newStartEnd.first;
        mFirstFrame = newStartEnd.first;
        mLastFrame = mFirstFrame + (frameCount - 1);

        bool reverse = (mSpeedMultiplier < 0.0f);

        mAnimTime = reverse ? mCurrentAnimation->getMaxTime() : mCurrentAnimation->getMinTime();

        mLastAppliedTransform = glm::dualquat(reverse ? mLastFrame->xform : mFirstFrame->xform);
    }

    void BoneAnimator::setAccumulationModes(const AxesAccumulationModes &modes)
    {
        for(size_t i = 0; i < 3; ++i)
        {
            switch(modes[i])
            {
            case AccumulationMode::BONE:
                mObjectAccumulationFactors[i] = 0.0;
                mBoneAccumulationFactors[i] = 1.0;
                break;

            case AccumulationMode::ACCUMULATE:
                mUseInterpolation = true; // accumulated motion should always be interpolated
                mObjectAccumulationFactors[i] = 1.0;
                mBoneAccumulationFactors[i] = 0.0;
                break;

            case AccumulationMode::IGNORE:
            default:
                mObjectAccumulationFactors[i] = 0.0;
                mBoneAccumulationFactors[i] = 0.0;
                break;
            }
        }
    }

    void BoneAnimator::update(float relTime)
    {
        if(!mPlaying || mCurrentAnimation == nullptr)
        {
            return;
        }

        mAnimTime += relTime * mSpeedMultiplier;

        bool loopedBack = false; // for correcting relative movement in case of a loop (unneeded when no accumulator used)
        glm::vec3 loopJump(0.0f);

        bool movingBackInTime = (mSpeedMultiplier < 0.0f);

        // have we moved beyond start/end of the current animation? if yes, we need to take appropriate actions before
        //  deciding how and where to move the bones, depending on whether we are looping, playing ping-pong etc.
        if((!movingBackInTime && mAnimTime > mCurrentAnimation->getMaxTime()) || (movingBackInTime && mAnimTime < mCurrentAnimation->getMinTime()))
        {
            // when looping or playing ping-pong, we want to add any time we have moved beyond the end of the animation
            //  back to the start/end of the timeline so we don't skip any frames.
            float startTime = movingBackInTime ? mCurrentAnimation->getMaxTime() : mCurrentAnimation->getMinTime();
            float endTime   = movingBackInTime ? mCurrentAnimation->getMinTime() : mCurrentAnimation->getMaxTime();
            float residualTime = mAnimTime - endTime; // negative for reverse playback!

            switch(mPlaybackType)
            {
            case PlaybackType::NORMAL:
            default:
                mPlaying = false;
                break;

            case PlaybackType::LOOPING:
                mAnimTime = startTime + residualTime;
                loopJump = _translationFrom3x4(mFirstFrame->xform) - _translationFrom3x4(mLastFrame->xform);
                loopJump *= movingBackInTime ? -1.0f : 1.0f;
                loopedBack = true;
                break;

            case PlaybackType::PINGPONG:
                mSpeedMultiplier = -mSpeedMultiplier;
                mAnimTime = endTime - residualTime;
                break;
            }
        }

        glm::dualquat sampledTransform = mUseInterpolation ? _sampleLinear(mAnimTime) : _sampleNearest(mAnimTime);

        if(mAccumulator == nullptr)
        {
            glm::mat4 asMat(glm::mat3x4_cast(sampledTransform));
            mBone.move(asMat);

        }else
        {
            glm::vec3 prevOffset = _translationFromDquat(mLastAppliedTransform);
            glm::vec3 currentOffset = _translationFromDquat(sampledTransform);

            glm::vec3 relativeOffset = currentOffset - prevOffset;

            // if the current animation step jumped in time, we need to factor out the offset
            //  between the last keyframe and the first (see diagram I drew which I keep in a drawer somewhere)
            if(loopedBack)
            {
                relativeOffset -= loopJump;
            }

            mAccumulator->moveRelative(relativeOffset * mObjectAccumulationFactors, relTime);

            glm::mat4 boneMatrix = glm::mat4_cast(sampledTransform.real); // real part represents rotation
            glm::vec3 boneTranslation = currentOffset * mBoneAccumulationFactors;
            boneMatrix[3] = glm::vec4(boneTranslation, 1.0);
            mBone.move(glm::transpose(boneMatrix));
        }

        mLastAppliedTransform = sampledTransform;
    }

    glm::dualquat BoneAnimator::_sampleLinear(float time)
    {
        odDb::Animation::KfIteratorPair currentKeyframes = mCurrentAnimation->getLeftAndRightKeyframe(mBone.getJointIndex(), time);

        if(currentKeyframes.first == currentKeyframes.second)
        {
            // clamped state. no need to interpolate
            return glm::dualquat(currentKeyframes.first->xform);
        }

        // we are are somewhere between keyframes, and have to interpolate

        if(mLastKeyframes != currentKeyframes) // only update decompositions when necessary
        {
            mLeftTransform = glm::dualquat(currentKeyframes.first->xform);
            mRightTransform = glm::dualquat(currentKeyframes.second->xform);
            mLastKeyframes = currentKeyframes;
        }

        // delta==0 -> exactly at current frame, delta==1 -> exactly at next frame
        float delta = (mAnimTime - currentKeyframes.first->time)/(currentKeyframes.second->time - currentKeyframes.first->time);

        return glm::lerp(mLeftTransform, mRightTransform, glm::clamp(delta, 0.0f, 1.0f));
    }

    glm::dualquat BoneAnimator::_sampleNearest(float time)
    {
        odDb::Animation::KfIteratorPair currentKeyframes = mCurrentAnimation->getLeftAndRightKeyframe(mBone.getJointIndex(), time);

        if(currentKeyframes.first == currentKeyframes.second)
        {
            // clamped state. no need to interpolate
            return glm::dualquat(currentKeyframes.first->xform);
        }

        // we are are somewhere between keyframes, and have to pick the closer one

        if(mLastKeyframes != currentKeyframes) // only update decompositions when necessary
        {
            bool firstIsCloser = (time - currentKeyframes.first->time) < (currentKeyframes.second->time - time);
            mLeftTransform = glm::dualquat(firstIsCloser ? currentKeyframes.first->xform : currentKeyframes.second->xform);
            mRightTransform = mLeftTransform;
            mLastKeyframes = currentKeyframes;
        }

        return mLeftTransform;
    }


    SkeletonAnimationPlayer::SkeletonAnimationPlayer(std::shared_ptr<Skeleton> skeleton)
    : mSkeleton(skeleton)
    , mPlaying(false)
    {
        if(mSkeleton == nullptr)
        {
            throw od::Exception("Tried to create SkeletonAnimationPlayer with null skeleton");
        }

        mBoneAnimators.reserve(mSkeleton->getBoneCount());
        for(size_t i = 0; i < mSkeleton->getBoneCount(); ++i)
        {
            mBoneAnimators.emplace_back(mSkeleton->getBoneByJointIndex(i));
        }
    }

    SkeletonAnimationPlayer::~SkeletonAnimationPlayer()
    {
    }

    void SkeletonAnimationPlayer::playAnimation(std::shared_ptr<odDb::Animation> anim,  PlaybackType type, float speedMultiplier)
    {
        for(auto &animator : mBoneAnimators)
        {
            animator.playAnimation(anim, type, speedMultiplier);
        }

        mPlaying = true;
    }

    void SkeletonAnimationPlayer::playAnimation(std::shared_ptr<odDb::Animation> anim, int32_t jointIndex, PlaybackType type, float speedMultiplier)
    {
        throw od::UnsupportedException("Partial skeleton animation not implemented yet");
    }

    void SkeletonAnimationPlayer::setNodeAccumulator(std::shared_ptr<MotionAccumulator> accu, int32_t nodeIndex)
    {
        if(nodeIndex < 0 || nodeIndex >= (int32_t)mBoneAnimators.size())
        {
            throw od::InvalidArgumentException("Node index out of bounds");
        }

        mBoneAnimators[nodeIndex].setAccumulator(accu);
    }

    void SkeletonAnimationPlayer::setNodeAccumulationModes(const AxesAccumulationModes &modes, int32_t nodeIndex)
    {
        if(nodeIndex < 0 || nodeIndex >= (int32_t)mBoneAnimators.size())
        {
            throw od::InvalidArgumentException("Node index out of bounds");
        }

        mBoneAnimators[nodeIndex].setAccumulationModes(modes);
    }

    bool SkeletonAnimationPlayer::update(float relTime)
    {
        if(!mPlaying)
        {
            return false;
        }

        bool stillPlaying = true;
        for(auto &animator : mBoneAnimators)
        {
            animator.update((float)relTime);
            stillPlaying |= animator.isPlaying();
        }

        if(mPlaying && !stillPlaying)
        {
            // stopped. might want to loop
            // TODO: invoke callback?
        }
        mPlaying = stillPlaying;

        return true; // last frame might still have changed the skeleton
    }

}
