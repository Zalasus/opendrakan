/*
 * Skeleton.h
 *
 *  Created on: 11 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_SKELETON_H_
#define INCLUDE_SKELETON_H_

#include <string>
#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/mat3x4.hpp>

namespace odDb
{
    class SkeletonDefinition;
}

namespace odRender
{
    class Rig;
}

namespace odAnim
{

    class Skeleton
    {
    public:

        class Bone
        {
        public:

            friend class Skeleton;

            Bone(Skeleton &skeleton, int32_t jointIndex);
            Bone(Bone &&bone) = default;

            inline const glm::mat4 &getCurrentTransform() const { return mCurrentMatrix; }
            inline Bone *getParent() { return mParent; }
            inline int32_t getJointIndex() const { return mJointIndex; }
            inline bool isRoot() const { return mParent == nullptr; }

            size_t getChildBoneCount();
            Bone &getChildBone(size_t index);
            Bone &addChildBone(int32_t jointIndex);

            void moveToBindPose();
            void move(const glm::mat4 &transform);

            template <typename F>
            void traverse(const F &f)
            {
                bool shouldContinue = f(*this);
                if(!shouldContinue)
                {
                    return;
                }

                for(auto bone : mChildBones)
                {
                    bone->traverse(f);
                }
            }


        private:

            void _flattenRecursive(odRender::Rig &rig, const glm::mat4 &parentMatrix);

            Skeleton &mSkeleton;
            Bone *mParent;
            int32_t mJointIndex;
            std::vector<Bone*> mChildBones;

            glm::mat4 mCurrentMatrix;
        };

        friend class Bone;

        explicit Skeleton(std::shared_ptr<odDb::SkeletonDefinition> def);
        Skeleton(const Skeleton &skeleton) = delete;

        inline size_t getBoneCount() const { return mBones.size(); }
        inline std::shared_ptr<const odDb::SkeletonDefinition> getDefinition() const { return mDefinition; }

        Bone &addRootBone(int32_t jointIndex);
        Bone &getBoneByJointIndex(int32_t jointIndex);
        Bone &getBoneByChannelIndex(int32_t channelIndex);

        template <typename F>
        void traverse(const F &f)
        {
            for(auto bone : mRootBones)
            {
                bone->traverse(f);
            }
        }

        void flatten(odRender::Rig &rig);
        bool checkForLoops(); ///< @brief Returns true if skeleton has loops


    private:

        std::shared_ptr<odDb::SkeletonDefinition> mDefinition;
        std::vector<Bone> mBones;
        std::vector<Bone*> mRootBones;

    };

}

#endif /* INCLUDE_SKELETON_H_ */
