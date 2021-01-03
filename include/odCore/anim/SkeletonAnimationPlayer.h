/*
 * SkeletonAnimationPlayer.h
 *
 *  Created on: 4 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_ANIM_SKELETONANIMATIONPLAYER_H_
#define INCLUDE_ODCORE_ANIM_SKELETONANIMATIONPLAYER_H_

#include <vector>
#include <memory>

#include <glm/gtx/norm.hpp> // needed due to missing include in glm/gtx/dual_quaternion.hpp, version 0.9.8.3-3
#include <glm/gtx/dual_quaternion.hpp>

#include <odCore/CTypes.h>

#include <odCore/db/Animation.h>

#include <odCore/anim/AnimModes.h>
#include <odCore/anim/Skeleton.h>

namespace odAnim
{
    class BoneAccumulator;

    class BoneAnimator
    {
    public:

        BoneAnimator(Skeleton::Bone &bone);
        ~BoneAnimator();

        inline Skeleton::Bone &getBone() { return mBone; }
        inline void setAccumulator(std::shared_ptr<BoneAccumulator> a) { mAccumulator = a; }
        inline std::shared_ptr<BoneAccumulator> getAccumulator() const { return mAccumulator; }
        inline const AxesBoneModes &getBoneModes() const { return mModes.boneModes; }
        inline bool isPlaying() const { return mPlaying; }
        inline std::shared_ptr<odDb::Animation> getCurrentAnimation() { return mCurrentAnimation; }

        /**
         * @brief Sets whether to use linear interpolation (true) or sample frames by nearest-neighbour (false).
         *
         * Original Drakan does not seem to use any sort of interpolation between frames, so false is the default.
         *
         * This setting is overridden to true if movement accumulation is enabled on any axis.
         */
        inline void setUseInterpolation(bool b) { mUseInterpolation = b; }

        void setBoneModes(const AxesBoneModes &modes);

        void playAnimation(std::shared_ptr<odDb::Animation> animation, const AnimModes &modes);

        /**
         * @brief Advances animation and performs necessary updates to the skeleton.
         *
         * @param  relTime      Relative time since the last update (realtime, will always be >= 0)
         */
        void update(float relTime);


    private:

        glm::dualquat _sampleLinear(std::shared_ptr<odDb::Animation> &anim, float time);
        glm::dualquat _sampleNearest(std::shared_ptr<odDb::Animation> &anim, float time);
        glm::dualquat _sample(std::shared_ptr<odDb::Animation> &anim, float time, bool interpolated);

        Skeleton::Bone &mBone;

        std::shared_ptr<odDb::Animation> mCurrentAnimation;
        AnimModes mModes;

        std::shared_ptr<odDb::Animation> mTransitionAnimation;
        AnimModes mTransitionModes;
        float mTransitionStartTime;

        bool mPlaying;
        float mPlayerTime;
        glm::vec3 mLoopJump;
        glm::dualquat mLastAppliedTransform;

        std::shared_ptr<BoneAccumulator> mAccumulator;
        AxesBoneModes mBoneModes;
        bool mHasNonDefaultBoneMode;
        bool mUseInterpolation;
    };


    class SkeletonAnimationPlayer
    {
    public:

        explicit SkeletonAnimationPlayer(std::shared_ptr<Skeleton> skeleton);
        ~SkeletonAnimationPlayer();

        inline bool isPlaying() const { return mPlaying; }

        /**
         * @brief Plays an animation.
         *
         * If the channelIndex in the modes struct is non-negative, the
         * animation will only play on the subtree starting from that channel.
         * This will override any playing animations on that subtree. You can
         * mix multiple animations this way, e.g. playing walking animation on
         * whole skeleton, then playing talking animation on neck channel to
         * make the character talk while walking.
         *
         * The speed field in modes may be negative for reverse playback. In
         * that case, the animation will start at it's last frame.
         *
         * Currently, the bone modes in the modes struct are ignored. You'll
         * have to set these manually via setBoneModes().
         */
        void playAnimation(std::shared_ptr<odDb::Animation> anim, const AnimModes &modes);

        /**
         * @brief Sets accumulator for a bone.
         *
         * If any axis of the target bone is set to BoneMode::ACCUMULATE, the relative movement of that bone
         * will be reported to the provided accumulator.
         */
        void setBoneAccumulator(std::shared_ptr<BoneAccumulator> accumulator, int32_t jointIndex);

        void setBoneModes(const AxesBoneModes &modes, int32_t jointIndex);

        std::shared_ptr<BoneAccumulator> getBoneAccumulator(int32_t jointIndex);
        const AxesBoneModes &getBoneModes(int32_t jointIndex);

        /**
         * @brief Advances the animation by relTime and applies changes to the skeleton.
         *
         * Returns true if any changes have been made to the skeleton (making flattening necessary), or false
         * if not.
         */
        bool update(float relTime);


    private:

        std::shared_ptr<Skeleton> mSkeleton;
        std::vector<BoneAnimator> mBoneAnimators; // indices in this correspond to bone/joint indices!
        bool mPlaying;
    };

}

#endif /* INCLUDE_ODCORE_ANIM_SKELETONANIMATIONPLAYER_H_ */
