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

            Bone(Skeleton &skeleton, Bone *parent, int32_t jointIndex);
            Bone(const Bone &bone);

            inline const std::string &getName() const { return mName; }
            inline const glm::mat4 &getInverseBinPoseTransform() const { return mInverseBindPoseTransform; }
            inline void setName(const std::string &name) { mName = name; }
            inline void setInverseBindPoseTransform(const glm::mat4 &tform) { mInverseBindPoseTransform = tform; }

            size_t getChildBoneCount();
            Bone *getChildBone(size_t index);
            Bone *addChildBone(int32_t jointIndex);

            void moveToBindPose();


        private:

            void _flattenRecursive(odRender::Rig *rig, const glm::mat4 &parentMatrix);

            Skeleton &mSkeleton;
            Bone *mParent;
            int32_t mJointIndex;
            std::string mName;   // TODO: the name and IBPT are the same for all instances of a skeleton. might want to save some memory here
            glm::mat4 mInverseBindPoseTransform;
            std::vector<Bone*> mChildBones;

            glm::mat4 mCurrentMatrix;
        };

        friend class Bone;

        explicit Skeleton(size_t initialBoneCapacity = 0);
        Skeleton(const Skeleton &skeleton) = delete;

        Bone *addRootBone(int32_t jointIndex);

        void flatten(odRender::Rig *rig);


    private:

        std::vector<Bone> mBones;
        std::vector<Bone*> mRootBones;

    };

}

#endif /* INCLUDE_SKELETON_H_ */
