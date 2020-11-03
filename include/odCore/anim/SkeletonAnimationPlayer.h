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
#include <odCore/anim/MotionAccumulator.h>

namespace odAnim
{

    class BoneAnimator
    {
    public:

        BoneAnimator(Skeleton::Bone &bone);

        inline Skeleton::Bone &getBone() { return mBone; }
        inline void setAccumulator(std::shared_ptr<MotionAccumulator> a) { mAccumulator = a; }
        inline bool isPlaying() const { return mPlaying; }
        inline std::shared_ptr<odDb::Animation> getCurrentAnimation() { return mCurrentAnimation; }

        /**
         * @brief Sets whether to use linear interpolation (true) or sample frames by nearest-neighbour (false).
         *
         * Original Drakan does not seem to use any sort of interpolation between frames, so false is the default.
         *
         * This is automatically set to true if movement accumulation is enabled on any axis.
         */
        inline void setUseInterpolation(bool b) { mUseInterpolation = b; }

        /**
         * @brief Instantly plays animation.
         *
         * This will set up the animator to play the passed animation as if the animation has been
         * started exactly after the last update.
         *
         * The speedModifier argument may be negative for reverse playback. In that case, the animation
         * will start at it's last frame.
         *
         * Only the current animation will be replaced; any animations in the queue will remain
         * untouched.
         *
         * @param type             The type of playback to use (normal, looping, pingpong)
         * @param speedMultiplier  Speed factor. 1.0 is normal playback speed. May be negative for reverse playback.
         */
        void playAnimation(std::shared_ptr<odDb::Animation> animation, PlaybackType type, float speedMultiplier);

        /**
         * @brief Advances animation and performs necessary updates to the skeleton.
         *
         * @param  relTime      Relative time since the last update (realtime, will always be >= 0)
         */
        void update(float relTime);

        void setAccumulationModes(const AxesAccumulationModes &modes);


    private:

        glm::dualquat _sampleLinear(float time);
        glm::dualquat _sampleNearest(float time);

        Skeleton::Bone &mBone;

        std::shared_ptr<odDb::Animation> mCurrentAnimation;
        PlaybackType mPlaybackType;
        float mSpeedMultiplier;
        odDb::Animation::KfIterator mFirstFrame;
        odDb::Animation::KfIterator mLastFrame;

        bool mPlaying;
        float mAnimTime;
        odDb::Animation::KfIteratorPair mLastKeyframes;
        glm::dualquat mLeftTransform;
        glm::dualquat mRightTransform;

        glm::dualquat mLastAppliedTransform;

        std::shared_ptr<MotionAccumulator> mAccumulator;
        glm::vec3 mBoneAccumulationFactors; // tells what part of translation is applied to bone
        glm::vec3 mObjectAccumulationFactors; // tells what part of translation is pushed to accumulator

        bool mUseInterpolation;
    };


    class SkeletonAnimationPlayer
    {
    public:

        explicit SkeletonAnimationPlayer(std::shared_ptr<Skeleton> skeleton);
        virtual ~SkeletonAnimationPlayer();

        inline bool isPlaying() const { return mPlaying; }

        /// @brief Plays animation on whole skeleton.
        void playAnimation(std::shared_ptr<odDb::Animation> anim, PlaybackType type, float speedMultiplier);

        /**
         * @brief Plays animation on skeleton subtree, starting at \c jointIndex.
         *
         * This will override any playing animations on that subtree. You can mix multiple
         * animations this way, e.g. playing walking animation on whole skeleton, then playing
         * talking animation on neck joint to make the character talk while walking.
         */
        void playAnimation(std::shared_ptr<odDb::Animation> anim, int32_t jointIndex, PlaybackType type, float speedMultiplier);

        /**
         * @brief Sets accumulator for a node.
         *
         * Setting this for a node overrides the default animation behaviour. Transforms will no
         * longer be pushed to the rig for that node, but instead be reported to the accumulator.
         *
         * Passing nullptr as accumulator returns the bone to it's default behavior.
         */
        void setNodeAccumulator(std::shared_ptr<MotionAccumulator> accumulator, int32_t nodeIndex);

        void setNodeAccumulationModes(const AxesAccumulationModes &modes, int32_t nodeIndex);

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
