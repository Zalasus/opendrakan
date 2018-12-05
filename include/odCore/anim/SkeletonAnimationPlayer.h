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

#include <odCore/render/ObjectNode.h>

namespace odAnim
{
    class BoneAnimator
    {
    public:

        BoneAnimator(Skeleton::Bone *bone);

        void setAnimation(odDb::Animation *animation);
        void play();
        void update(float relTime);


    private:

        Skeleton::Bone *mBone;
        od::RefPtr<odDb::Animation> mCurrentAnimation;
        odDb::Animation::AnimStartEndPair mKeyframesStartEnd;

        bool mPlaying;
        float mCurrentTime;
        odDb::Animation::AnimKfIterator mCurrentKeyframe;
        glm::dualquat mLeftTransform;
        glm::dualquat mRightTransform;
    };

    class SkeletonAnimationPlayer : public odRender::FrameListener
    {
    public:

        explicit SkeletonAnimationPlayer(odRender::ObjectNode *objectNode, Skeleton *skeleton);
        ~SkeletonAnimationPlayer();

        /// @brief Plays animation on whole skeleton.
        void playAnimation(odDb::Animation *anim);

        /**
         * @brief Plays animation on skeleton subtree, starting at \c jointIndex.
         *
         * This will override any playing animations on that subtree. You can mix multiple
         * animations this way, e.g. playing walking animation on whole skeleton, then playing
         * talking animation on neck joint to make the character talk while walking.
         */
        void playAnimation(odDb::Animation *anim, int32_t jointIndex);

        // implement odRender::FrameListener
        virtual void onFrameUpdate(double simTime, double relTime, uint32_t frameNumber) override;


    private:

        odRender::ObjectNode *mObjectNode;
        Skeleton *mSkeleton;
        std::vector<BoneAnimator> mBoneAnimators;
    };

}

#endif /* INCLUDE_ODCORE_ANIM_SKELETONANIMATIONPLAYER_H_ */
