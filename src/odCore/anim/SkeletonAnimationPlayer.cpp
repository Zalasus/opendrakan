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


    BoneAnimator::BoneAnimator(Skeleton::Bone *bone)
    : mBone(bone)
    , mPlaybackType(PlaybackType::Normal)
    , mSpeedMultiplier(1.0f)
    , mPlaying(false)
    , mAnimTime(0.0f)
    , mIsInPongPhase(false)
    , mMadeNonContinousJump(false)
    , mAccumulator(nullptr)
    , mBoneAccumulationFactors(0.0)
    , mObjectAccumulationFactors(0.0)
    {
        if(mBone == nullptr)
        {
            throw od::Exception("Created BoneAnimator with bone = nullptr");
        }
    }

    void BoneAnimator::playAnimation(odDb::Animation *animation, PlaybackType type, float speedMultiplier)
    {
        if(animation == nullptr)
        {
            mPlaying = false;
            mCurrentAnimation = nullptr;
            return;
        }

        /*odDb::Animation::KfIteratorPair newStartEnd = animation->getKeyframesForNode(mBone->getJointIndex());
        odDb::Animation::KfIterator firstKfOfNewAnim = speedMultiplier < 0.0f ? newStartEnd.second : newStartEnd.first;

        if(mCurrentAnimation != nullptr)
        {
            odDb::Animation::KfIterator lastKfOfOldAnim = (((mSpeedMultiplier < 0.0f) ^ mIsInPongPhase)) ? mAnimStartEnd.first : mAnimStartEnd.second;

            mNonContinousOffset = _translationFrom3x4(firstKfOfNewAnim->xform) - _translationFrom3x4(lastKfOfOldAnim->xform);
            mMadeNonContinousJump = true;
        }*/

        mCurrentAnimation = animation;
        mPlaybackType = type;
        mSpeedMultiplier = speedMultiplier;
        mIsInPongPhase = false;
        mPlaying = true;

        odDb::Animation::KfIteratorPair newStartEnd = animation->getKeyframesForNode(mBone->getJointIndex());
        size_t frameCount = newStartEnd.second - newStartEnd.first;
        mFirstFrame = newStartEnd.first;
        mLastFrame = mFirstFrame + (frameCount - 1);

        mAnimTime = mSpeedMultiplier < 0.0f ? mCurrentAnimation->getMaxTime() : mCurrentAnimation->getMinTime();
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

    void BoneAnimator::update(float relTime)
    {
        if(!mPlaying || mCurrentAnimation == nullptr)
        {
            return;
        }

        mAnimTime += relTime * mSpeedMultiplier * (mIsInPongPhase ? -1.0f : 1.0f);

        odDb::Animation::KfIteratorPair currentKeyframes = mCurrentAnimation->getLeftAndRightKeyframe(mBone->getJointIndex(), mAnimTime);

        if(currentKeyframes.first == currentKeyframes.second)
        {
            // we are outside of the defined animation timeline. depending on playback type and position, we need
            //  to loop or stop playback.

            bool movingBackInTime = (mSpeedMultiplier < 0) ^ mIsInPongPhase;

            float startTime = movingBackInTime ? mCurrentAnimation->getMaxTime() : mCurrentAnimation->getMinTime();
            float endTime   = movingBackInTime ? mCurrentAnimation->getMinTime() : mCurrentAnimation->getMaxTime();

            // although this timeline might have ended, we might not yet be at the end of the animation (other bones might
            //  still need to finish). if we are still within the animation limits, ...
            if(    (!movingBackInTime && mAnimTime < mCurrentAnimation->getMaxTime())
                || ( movingBackInTime && mAnimTime > mCurrentAnimation->getMinTime()))
            {
                return;
            }

            // when looping or playing ping-pong, we want to add any time we have moved beyond the end of the animation
            //  back to the start/end of the timeline so we don't skip any frames.
            float residualTime = mAnimTime - endTime;

            switch(mPlaybackType)
            {
            case PlaybackType::Normal:
            default:
                mPlaying = false;
                break;

            case PlaybackType::Looping:
                mAnimTime = startTime + residualTime;
                currentKeyframes = mCurrentAnimation->getLeftAndRightKeyframe(mBone->getJointIndex(), mAnimTime);
                mNonContinousOffset = _translationFrom3x4(mFirstFrame->xform) - _translationFrom3x4(mLastFrame->xform);
                mMadeNonContinousJump = true;
                break;

            case PlaybackType::PingPong:
                mIsInPongPhase = !mIsInPongPhase;
                mAnimTime = endTime - residualTime;
                currentKeyframes = mCurrentAnimation->getLeftAndRightKeyframe(mBone->getJointIndex(), mAnimTime);
                break;
            }
        }

        // the above processing of a beyond-timeline state could either have left us there, or moved us back into
        //  a defined state. depending on that we might have to take different approaches to moving the bone
        if(currentKeyframes.first == currentKeyframes.second)
        {
            // we are still in a clamped state. simply apply the transform without interpolating
            mBone->move(glm::mat4(currentKeyframes.first->xform));
            return;
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

            // if the current animation step jumped in time, we need to factor out the offset
            //  between the last keyframe and the first (see diagram I drew which I keep in a drawer somewhere)
            if(mMadeNonContinousJump)
            {
                relativeOffset -= mNonContinousOffset;
                mMadeNonContinousJump = false;
            }

            mAccumulator->moveRelative(relativeOffset * mObjectAccumulationFactors, relTime);

            glm::mat4 boneMatrix = glm::mat4_cast(interpolatedTransform.real); // real part represents rotation
            glm::vec3 boneTranslation = currentOffset * mBoneAccumulationFactors;
            boneMatrix[3] = glm::vec4(boneTranslation, 1.0);
            mBone->move(glm::transpose(boneMatrix));
        }

        mLastAppliedTransform = interpolatedTransform;
    }


    SkeletonAnimationPlayer::SkeletonAnimationPlayer(odRender::ObjectNode *objectNode, Skeleton *skeleton)
    : mObjectNode(objectNode)
    , mSkeleton(skeleton)
    , mRig(nullptr)
    , mPlaying(false)
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

    void SkeletonAnimationPlayer::playAnimation(odDb::Animation *anim,  PlaybackType type, float speedMultiplier)
    {
        for(auto it = mBoneAnimators.begin(); it != mBoneAnimators.end(); ++it)
        {
            it->playAnimation(anim, type, speedMultiplier);
        }

        mPlaying = true;
    }

    void SkeletonAnimationPlayer::playAnimation(odDb::Animation *anim, int32_t jointIndex, PlaybackType type, float speedMultiplier)
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

        bool stillPlaying = true;
        for(auto it = mBoneAnimators.begin(); it != mBoneAnimators.end(); ++it)
        {
            it->update((float)relTime);
            stillPlaying |= it->isPlaying();
        }

        if(mPlaying && !stillPlaying)
        {
            // stopped. might want to loop
            // TODO: invoke callback?
        }
        mPlaying = stillPlaying;

        // note: even if we are no longer playing by now, we still might need to flatten the last frame.
        //   thus, we don't check for the playing flag here
        if(mRig != nullptr)
        {
            mSkeleton->flatten(mRig);
        }
    }

}
