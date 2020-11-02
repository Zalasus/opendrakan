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

#include <glm/mat4x4.hpp>
#include <glm/mat3x4.hpp>

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
            Bone(const Bone &bone);

            inline const std::string &getName() const { return mName; }
            inline const glm::mat4 &getInverseBindPoseTransform() const { return mInverseBindPoseTransform; }
            inline void setName(const std::string &name) { mName = name; }
            inline const glm::mat4 &getCurrentTransform() const { return mCurrentMatrix; }
            inline Bone *getParent() { return mParent; }
            inline int32_t getJointIndex() const { return mJointIndex; }
            inline bool isRoot() const { return mParent == nullptr; }

            void setInverseBindPoseTransform(const glm::mat4 &tform);

            size_t getChildBoneCount();
            Bone &getChildBone(size_t index);
            Bone &addChildBone(int32_t jointIndex);

            void moveToBindPose();
            void move(const glm::mat4 &transform);


        private:

            void _flattenRecursive(odRender::Rig &rig, const glm::mat4 &parentMatrix);

            template <typename F>
            void _traverse(const F &f)
            {
                bool shouldContinue = f(*this);
                if(!shouldContinue)
                {
                    return;
                }

                for(auto bone : mChildBones)
                {
                    bone->_traverse(f);
                }
            }

            Skeleton &mSkeleton;
            Bone *mParent;
            int32_t mJointIndex;
            std::string mName;   // TODO: the name and IBPT are the same for all instances of a skeleton. might want to save some memory here
            glm::mat4 mInverseBindPoseTransform;
            glm::mat4 mBindPoseTransform;
            std::vector<Bone*> mChildBones;

            glm::mat4 mCurrentMatrix;
        };

        friend class Bone;

        explicit Skeleton(size_t boneCount);
        Skeleton(const Skeleton &skeleton) = delete;

        inline size_t getBoneCount() const { return mBones.size(); }

        Bone &addRootBone(int32_t jointIndex);
        Bone &getBoneByJointIndex(int32_t jointIndex);

        template <typename F>
        void traverse(const F &f)
        {
            for(auto bone : mRootBones)
            {
                bone->_traverse(f);
            }
        }

        void flatten(odRender::Rig &rig);
        bool checkForLoops(); ///< @brief Returns true if skeleton has loops


    private:

        std::vector<Bone> mBones;
        std::vector<Bone*> mRootBones;

    };

}

#endif /* INCLUDE_SKELETON_H_ */
