/*
 * SkeletonAnimationPlayer.h
 *
 *  Created on: 4 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_ANIM_SKELETONANIMATIONPLAYER_H_
#define INCLUDE_ODCORE_ANIM_SKELETONANIMATIONPLAYER_H_

#include <stdint.h>
#include <vector>

#include <glm/gtx/norm.hpp> // needed due to missing include in glm/gtx/dual_quaternion.hpp, version 0.9.8.3-3
#include <glm/gtx/dual_quaternion.hpp>

#include <odCore/RefCounted.h>

#include <odCore/db/Animation.h>

#include <odCore/anim/Skeleton.h>
#include <odCore/anim/MotionAccumulator.h>

#include <odCore/render/ObjectNode.h>

namespace odAnim
{

    class BoneAnimator
    {
    public:

        BoneAnimator(Skeleton::Bone *bone);

        inline Skeleton::Bone *getBone() { return mBone; }
        inline void setAccumulator(MotionAccumulator *accu) { mAccumulator = accu; }
        inline bool isPlaying() const { return mPlaying; }

        void setAnimation(odDb::Animation *animation);
        void play(bool looping);

        /**
         * Advanced animation and performs necessary updates to the skeleton.
         * @return true if skeleton has been moved and needs to be flattened.
         */
        bool update(float relTime);

        void setAccumulationModes(const AxesModes &modes);


    private:

        Skeleton::Bone *mBone;
        od::RefPtr<odDb::Animation> mCurrentAnimation;
        odDb::Animation::AnimStartEndPair mKeyframesStartEnd;

        bool mPlaying;
        bool mLooping;
        float mCurrentTime;
        odDb::Animation::AnimKfIterator mCurrentKeyframe;
        glm::dualquat mLeftTransform;
        glm::dualquat mRightTransform;

        glm::dualquat mPreviousTransform; // last applied transform

        MotionAccumulator *mAccumulator;
        glm::vec3 mBoneAccumulationFactors; // tells what part of translation is applied to bone
        glm::vec3 mObjectAccumulationFactors; // tells what part of translation is pushed to accumulator
    };


    class SkeletonAnimationPlayer : public odRender::FrameListener
    {
    public:

        explicit SkeletonAnimationPlayer(odRender::ObjectNode *objectNode, Skeleton *skeleton);
        ~SkeletonAnimationPlayer();

        inline bool isPlaying() const { return mPlaying; }

        /// @brief Plays animation on whole skeleton.
        void playAnimation(odDb::Animation *anim, bool looping);

        /**
         * @brief Plays animation on skeleton subtree, starting at \c jointIndex.
         *
         * This will override any playing animations on that subtree. You can mix multiple
         * animations this way, e.g. playing walking animation on whole skeleton, then playing
         * talking animation on neck joint to make the character talk while walking.
         */
        void playAnimation(odDb::Animation *anim, int32_t jointIndex, bool looping);

        /**
         * @brief Sets accumulator for a root node.
         *
         * Setting this for a node overrides the default animation behaviour. Transforms will no
         * longer be pushed to the rig for that node, but instead be reported to the accumulator.
         *
         * Passing nullptr as accumulator returns the bone to it's default behavior.
         *
         * The \c rootNodeIndex parameter determines which root node to assign the accumulator to
         * for skeletons with multiple roots. This can be ignored most of the time.
         */
        void setRootNodeAccumulator(MotionAccumulator *accu, int32_t rootNodeIndex = 0);

        void setRootNodeAccumulationModes(const AxesModes &modes, int32_t rootNodeIndex = 0);

        // implement odRender::FrameListener
        virtual void onFrameUpdate(double simTime, double relTime, uint32_t frameNumber) override;


    private:

        od::RefPtr<odRender::ObjectNode> mObjectNode;
        Skeleton *mSkeleton;
        odRender::Rig *mRig;
        std::vector<BoneAnimator> mBoneAnimators; // indices in this correspond to bone/joint indices!
        bool mPlaying;
    };

}

#endif /* INCLUDE_ODCORE_ANIM_SKELETONANIMATIONPLAYER_H_ */
