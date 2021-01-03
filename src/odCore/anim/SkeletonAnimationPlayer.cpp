/*
 * SkeletonAnimationPlayer.cpp
 *
 *  Created on: 4 Dec 2018
 *      Author: zal
 */

#include <odCore/anim/SkeletonAnimationPlayer.h>

#include <odCore/Logger.h>
#include <odCore/Exception.h>

#include <odCore/anim/BoneAccumulator.h>

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
    , mTransitionAnimation(nullptr)
    , mTransitionStartTime(0.0f)
    , mPlaying(false)
    , mPlayerTime(0.0f)
    , mBoneModes({BoneMode::NORMAL, BoneMode::NORMAL, BoneMode::NORMAL})
    , mHasNonDefaultBoneMode(false)
    , mUseInterpolation(false)
    {
    }

    BoneAnimator::~BoneAnimator()
    {
    }

    void BoneAnimator::setBoneModes(const AxesBoneModes &modes)
    {
        mBoneModes = modes;

        mHasNonDefaultBoneMode = false;
        for(auto mode : modes)
        {
            if(mode != BoneMode::NORMAL)
            {
                mHasNonDefaultBoneMode = true;
                break;
            }
        }
    }

    void BoneAnimator::playAnimation(std::shared_ptr<odDb::Animation> animation, const AnimModes &modes)
    {
        if(false && modes.transitionTime > 0.0f)
        {
            mTransitionAnimation = mCurrentAnimation;
            mTransitionStartTime = mPlayerTime;
            mTransitionModes = mModes;
        }

        if(animation == nullptr)
        {
            mPlaying = false;
            mCurrentAnimation = nullptr;
            return;
        }

        mCurrentAnimation = animation;
        mModes = modes;
        mPlaying = true;
        mPlayerTime = 0.0f;

        bool reverse = (mModes.speed < 0.0f);

        odDb::Animation::KfIteratorPair newStartEnd = animation->getKeyframesForNode(mBone.getJointIndex());
        size_t frameCount = newStartEnd.second - newStartEnd.first;
        auto firstFrame = newStartEnd.first;
        auto lastFrame = firstFrame + (frameCount - 1);
        mLastAppliedTransform = glm::dualquat(reverse ? lastFrame->xform : firstFrame->xform);
        mLoopJump = _translationFrom3x4(firstFrame->xform) - _translationFrom3x4(lastFrame->xform);
        if(reverse)
        {
            mLoopJump *= -1.0f;
        }
    }

    static float _linearToAnimTime(float duration, const AnimModes &modes, float time)
    {
        float animTime = (modes.speed >= 0.0f) ? (time*modes.speed) : (duration + time*modes.speed);
        animTime += modes.startTime;

        switch(modes.playbackType)
        {
        case PlaybackType::NORMAL:
            return glm::clamp(animTime, 0.0f, duration);

        case PlaybackType::LOOPING:
            return std::fmod(animTime, duration);

        case PlaybackType::PINGPONG:
            if(static_cast<int32_t>(animTime/duration) % 2 == 0)
            {
                // forward phase
                return std::fmod(animTime, duration);

            }else
            {
                // backward phase
                return duration - std::fmod(animTime, duration);
            }
        }

        OD_UNREACHABLE();
    }

    void BoneAnimator::update(float relTime)
    {
        if(!mPlaying || mCurrentAnimation == nullptr)
        {
            return;
        }

        float prevPlayerTime = mPlayerTime;
        mPlayerTime += relTime;

        // for correcting relative movement in case of a loop
        bool loopedBack = false;

        float animTime = _linearToAnimTime(mCurrentAnimation->getDuration(), mModes, mPlayerTime);

        // have we moved beyond start/end of the current animation? if yes, we need to take appropriate actions before
        //  deciding how and where to move the bones, depending on whether we are looping, playing ping-pong etc.
        switch(mModes.playbackType)
        {
        case PlaybackType::NORMAL:
            if(mPlayerTime >= mCurrentAnimation->getDuration())
            {
                mPlaying = false;
            }
            break;

        case PlaybackType::LOOPING:
            {
                float prevAnimTime = _linearToAnimTime(mCurrentAnimation->getDuration(), mModes, prevPlayerTime);
                loopedBack = (animTime < prevAnimTime);
            }
            break;

        case PlaybackType::PINGPONG:
            break;
        }

        bool needInterpolation = mUseInterpolation || (mAccumulator != nullptr); // accumulated motion should always be interpolated
        glm::dualquat sampledTransform = _sample(mCurrentAnimation, animTime, needInterpolation);

        if(mTransitionAnimation != nullptr)
        {
            float transitionAnimTime = _linearToAnimTime(mTransitionAnimation->getDuration(), mTransitionModes, mTransitionStartTime + mPlayerTime);
            float transitionDelta = (mPlayerTime - mTransitionStartTime) / mModes.transitionTime;
            glm::dualquat sampledTransitionTransform = _sample(mTransitionAnimation, transitionAnimTime, needInterpolation);
            sampledTransform = glm::lerp(sampledTransitionTransform, sampledTransform, glm::clamp(transitionDelta, 0.0f, 1.0f));
            if(transitionDelta >= 1.0f)
            {
                mTransitionAnimation = nullptr;
            }
        }

        if(!mHasNonDefaultBoneMode)
        {
            glm::mat4 asMat(glm::mat3x4_cast(sampledTransform));
            mBone.move(asMat);

        }else
        {
            glm::vec3 currentOffset = _translationFromDquat(sampledTransform);
            glm::vec3 prevOffset = _translationFromDquat(mLastAppliedTransform);
            glm::vec3 relativeOffset = currentOffset - prevOffset;

            // if the current animation step jumped in time, we need to factor out the offset
            //  between the last keyframe and the first (see diagram I drew which I keep in a drawer somewhere)
            if(loopedBack)
            {
                relativeOffset -= mLoopJump;
            }

            glm::vec3 boneTranslation = currentOffset;
            glm::vec3 accumulatorTranslation = relativeOffset;
            for(size_t i = 0; i < 3; ++i)
            {
                switch(mBoneModes[i])
                {
                case BoneMode::NORMAL:
                    accumulatorTranslation[i] = 0.0f;
                    break;

                case BoneMode::ACCUMULATE:
                    boneTranslation[i] = 0.0f;
                    break;

                case BoneMode::IGNORE:
                    boneTranslation[i] = 0.0f;
                    accumulatorTranslation[i] = 0.0f;
                    break;
                }
            }

            if(mAccumulator != nullptr)
            {
                mAccumulator->moveRelative(accumulatorTranslation, relTime);
            }

            glm::mat4 boneMatrix = glm::mat4_cast(sampledTransform.real); // real part represents rotation
            boneMatrix[3] = glm::vec4(boneTranslation, 1.0f);
            mBone.move(glm::transpose(boneMatrix));
        }

        mLastAppliedTransform = sampledTransform;
    }

    glm::dualquat BoneAnimator::_sampleLinear(std::shared_ptr<odDb::Animation> &anim, float time)
    {
        auto currentKeyframes = anim->getLeftAndRightKeyframe(mBone.getJointIndex(), time);

        if(currentKeyframes.first == currentKeyframes.second)
        {
            // clamped state. no need to interpolate
            return glm::dualquat(currentKeyframes.first->xform);
        }

        // we are are somewhere between keyframes, and have to interpolate
        //  TODO: cache decompositions
        glm::dualquat leftTransform(currentKeyframes.first->xform);
        glm::dualquat rightTransform(currentKeyframes.second->xform);

        // delta==0 -> exactly at current frame, delta==1 -> exactly at next frame
        float delta = (time - currentKeyframes.first->time)/(currentKeyframes.second->time - currentKeyframes.first->time);

        return glm::lerp(leftTransform, rightTransform, glm::clamp(delta, 0.0f, 1.0f));
    }

    glm::dualquat BoneAnimator::_sampleNearest(std::shared_ptr<odDb::Animation> &anim, float time)
    {
        auto currentKeyframes = anim->getLeftAndRightKeyframe(mBone.getJointIndex(), time);

        if(currentKeyframes.first == currentKeyframes.second)
        {
            // clamped state. no need to interpolate
            return glm::dualquat(currentKeyframes.first->xform);
        }

        // we are are somewhere between keyframes, and have to pick the closer one
        //  TODO: cache decompositions
        bool firstIsCloser = (time - currentKeyframes.first->time) < (currentKeyframes.second->time - time);
        return glm::dualquat(firstIsCloser ? currentKeyframes.first->xform : currentKeyframes.second->xform);
    }

    glm::dualquat BoneAnimator::_sample(std::shared_ptr<odDb::Animation> &anim, float time, bool interpolated)
    {
         return interpolated ? _sampleLinear(anim, time) : _sampleNearest(anim, time);
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

    void SkeletonAnimationPlayer::playAnimation(std::shared_ptr<odDb::Animation> anim, const AnimModes &modes)
    {
        if(modes.channel < 0)
        {
            // play on whole skeleton
            for(auto &animator : mBoneAnimators)
            {
                animator.playAnimation(anim, modes);
            }

        }else
        {
            auto &bone = mSkeleton->getBoneByChannelIndex(modes.channel);
            auto p = [this, anim, modes](odAnim::Skeleton::Bone &b)
            {
                mBoneAnimators[b.getJointIndex()].playAnimation(anim, modes);
                return true;
            };
            bone.traverse(p);
        }

        mPlaying = true;
    }

    void SkeletonAnimationPlayer::setBoneAccumulator(std::shared_ptr<BoneAccumulator> accu, int32_t nodeIndex)
    {
        mBoneAnimators.at(nodeIndex).setAccumulator(accu);
    }

    void SkeletonAnimationPlayer::setBoneModes(const AxesBoneModes &modes, int32_t nodeIndex)
    {
        mBoneAnimators.at(nodeIndex).setBoneModes(modes);
    }

    std::shared_ptr<BoneAccumulator> SkeletonAnimationPlayer::getBoneAccumulator(int32_t jointIndex)
    {
        return mBoneAnimators.at(jointIndex).getAccumulator();
    }

    const AxesBoneModes &SkeletonAnimationPlayer::getBoneModes(int32_t jointIndex)
    {
        return mBoneAnimators.at(jointIndex).getBoneModes();
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
