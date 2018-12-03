/*
 * Skeleton.h
 *
 *  Created on: 11 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_SKELETON_H_
#define INCLUDE_DB_SKELETON_H_

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/mat3x4.hpp>

namespace odRender
{
    class Rig;
}

namespace odDb
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


    class SkeletonBuilder
    {
    public:

        SkeletonBuilder();

        inline size_t getJointCount() const { return mJointInfos.size(); }

        void addJointNameInfo(const std::string &name, int32_t jointIndex);
        void addJointInfo(const glm::mat4 &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
        void markJointAsChannel(size_t jointIndex);

        void build(Skeleton &skeleton);


    private:

        struct JointNameInfo
        {
            std::string name;
            int32_t jointIndex;
        };

        struct JointInfo
        {
            glm::mat4 boneXform;
            int32_t meshIndex;
            int32_t firstChildIndex;
            int32_t nextSiblingIndex;

            JointNameInfo *nameInfo;
            bool isChannel;

            bool visited;
        };

        void _buildRecursive(Skeleton::Bone *parent, JointInfo *jointInfo, int32_t jointIndex);

        std::vector<JointNameInfo> mNameInfos;
        std::vector<JointInfo> mJointInfos;
        bool mAlreadyBuiltNameLinks;
    };

}

#endif /* INCLUDE_DB_SKELETON_H_ */
