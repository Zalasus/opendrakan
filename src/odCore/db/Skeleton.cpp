/*
 * Skeleton.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include <odCore/db/Skeleton.h>

#include <algorithm>

#include <odCore/Exception.h>

namespace odDb
{

    Skeleton::Bone::Bone(Skeleton &skeleton)
    : mSkeleton(skeleton)
    {
    }

    Skeleton::Bone::Bone(const Bone &bone)
    : mSkeleton(bone.mSkeleton)
    , mName(bone.mName)
    , mInverseBindPoseTransform(bone.mInverseBindPoseTransform)
    , mChildBones(bone.mChildBones)
    , mCurrentMatrix(mInverseBindPoseTransform)
    {
    }

    size_t Skeleton::Bone::getChildBoneCount()
    {
        return mChildBones.size();
    }

    Skeleton::Bone *Skeleton::Bone::getChildBone(size_t index)
    {
        if(index >= mChildBones.size())
        {
            throw od::Exception("Child bone index out of bounds");
        }

        return mChildBones[index];
    }

    Skeleton::Bone *Skeleton::Bone::addChildBone()
    {
        mSkeleton.mBones.push_back(Bone(mSkeleton));
        mChildBones.push_back(&mSkeleton.mBones.back());
        return mChildBones.back();
    }

    void Skeleton::Bone::moveToBindPose()
    {
        mCurrentMatrix = glm::inverse(mInverseBindPoseTransform);
    }


    Skeleton::Skeleton(size_t initialBoneCapacity)
    {
        mBones.reserve(initialBoneCapacity);
    }

    Skeleton::Skeleton(const Skeleton &skeleton)
    : mBones(skeleton.mBones)
    {
    }

    Skeleton::Bone *Skeleton::addRootBone()
    {
        mBones.push_back(Bone(*this));

        return &mBones.back();
    }



    SkeletonBuilder::SkeletonBuilder()
    : mAlreadyBuiltNameLinks(false)
    {
    }

    void SkeletonBuilder::addJointNameInfo(const std::string &name, int32_t jointIndex)
    {
        JointNameInfo info;
        info.name = name;
        info.jointIndex = jointIndex;

        mNameInfos.push_back(info);
    }

    void SkeletonBuilder::addJointInfo(const glm::mat4 &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex)
    {
        JointInfo info;
        info.boneXform = boneXform;
        info.meshIndex = meshIndex;
        info.firstChildIndex = firstChildIndex;
        info.nextSiblingIndex = nextSiblingIndex;

        mJointInfos.push_back(info);
    }

    void SkeletonBuilder::markJointAsChannel(size_t jointIndex)
    {
        if(jointIndex >= mJointInfos.size())
        {
            throw od::Exception("Channel index in skeleton out of joint index range");
        }

        mJointInfos[jointIndex].isChannel = true;
    }

    void SkeletonBuilder::build(Skeleton &skeleton)
    {
        if(!mAlreadyBuiltNameLinks)
        {
            for(auto it = mNameInfos.begin(); it != mNameInfos.end(); ++it)
            {
                if(it->jointIndex < 0)
                {
                    continue;

                }else if(it->jointIndex >= mJointInfos.size())
                {
                    throw od::Exception("Joint name info's referenced joint index is out of bounds");
                }

                mJointInfos[it->jointIndex].nameInfo = &(*it);
            }

            mAlreadyBuiltNameLinks = true;
        }

        // by iterating over all joints and starting to build from all untouched joints we encounter, we can
        //  also handle skeletons that have more than one root for some reason
        for(auto it = mJointInfos.begin(); it != mJointInfos.end(); ++it)
        {
            if(it->visited)
            {
                continue;
            }

            Skeleton::Bone *root = skeleton.addRootBone();
            _buildRecursive(root, &(*it));
        }
    }

    void SkeletonBuilder::_buildRecursive(Skeleton::Bone *parent, JointInfo *jointInfo)
    {
        if(parent == nullptr || jointInfo == nullptr)
        {
            return;
        }

        if(jointInfo->visited)
        {
            throw od::Exception("Encountered loop in bone tree");

        }else
        {
            jointInfo->visited = true;
        }

        Skeleton::Bone *bone = parent->addChildBone();
        bone->setInverseBindPoseTransform(jointInfo->boneXform);
        if(jointInfo->nameInfo != nullptr)
        {
            bone->setName(jointInfo->nameInfo->name);
        }

        if(jointInfo->firstChildIndex > 0)
        {
            if(jointInfo->firstChildIndex >= (int32_t)mJointInfos.size())
            {
                throw od::Exception("First child index in joint info out of bounds");
            }

            JointInfo *firstChildJointInfo = &mJointInfos[jointInfo->firstChildIndex];
            _buildRecursive(bone, firstChildJointInfo);
        }

        if(jointInfo->nextSiblingIndex > 0)
        {
            if(jointInfo->nextSiblingIndex >= (int32_t)mJointInfos.size())
            {
                throw od::Exception("Next sibling index in joint info out of bounds");
            }

            JointInfo *nextSiblingJointInfo = &mJointInfos[jointInfo->nextSiblingIndex];
            _buildRecursive(parent, nextSiblingJointInfo);
        }
    }

}
