/*
 * Skeleton.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include <odCore/db/Skeleton.h>

#include <algorithm>

#include <glm/matrix.hpp>

#include <odCore/Exception.h>

#include <odCore/render/Rig.h>

namespace odDb
{

    Skeleton::Bone::Bone(Skeleton &skeleton, Bone *parent, int32_t jointIndex)
    : mSkeleton(skeleton)
    , mParent(parent)
    , mJointIndex(jointIndex)
    {
        moveToBindPose();
    }

    Skeleton::Bone::Bone(const Bone &bone)
    : mSkeleton(bone.mSkeleton)
    , mParent(bone.mParent)
    , mJointIndex(bone.mJointIndex)
    , mName(bone.mName)
    , mInverseBindPoseTransform(bone.mInverseBindPoseTransform)
    {
        if(bone.mChildBones.size() > 0)
        {
            throw od::Exception("Copying bone with set child links");
        }

        moveToBindPose();
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

    Skeleton::Bone *Skeleton::Bone::addChildBone(int32_t jointIndex)
    {
        mSkeleton.mBones.push_back(Bone(mSkeleton, this, jointIndex));
        mChildBones.push_back(&mSkeleton.mBones.back());
        return mChildBones.back();
    }

    void Skeleton::Bone::moveToBindPose()
    {
        mCurrentMatrix = glm::inverse(mInverseBindPoseTransform);
    }

    void Skeleton::Bone::_flattenRecursive(odRender::Rig *rig, const glm::mat4 &parentMatrix)
    {
        glm::mat4 currentMatrix = parentMatrix * mCurrentMatrix * mInverseBindPoseTransform;

        rig->setBoneTransform(mJointIndex, currentMatrix);

        for(auto it = mChildBones.begin(); it != mChildBones.end(); ++it)
        {
            (*it)->_flattenRecursive(rig, currentMatrix);
        }
    }


    Skeleton::Skeleton(size_t initialBoneCapacity)
    {
        mBones.reserve(initialBoneCapacity);
        mRootBones.reserve(2);
    }

    Skeleton::Bone *Skeleton::addRootBone(int32_t jointIndex)
    {
        mBones.push_back(Bone(*this, nullptr, jointIndex));
        mRootBones.push_back(&mBones.back());
        return mRootBones.back();
    }

    void Skeleton::flatten(odRender::Rig *rig)
    {
        glm::mat4 eye(1.0);

        for(auto it = mRootBones.begin(); it != mRootBones.end(); ++it)
        {
            (*it)->_flattenRecursive(rig, eye);
        }
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

                }else if(it->jointIndex >= (int32_t)mJointInfos.size())
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

            int32_t jointIndex = it - mJointInfos.begin();
            Skeleton::Bone *root = skeleton.addRootBone(jointIndex);
            _buildRecursive(root, &(*it), jointIndex);
        }
    }

    void SkeletonBuilder::_buildRecursive(Skeleton::Bone *parent, JointInfo *jointInfo, int32_t jointIndex)
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

        Skeleton::Bone *bone = parent->addChildBone(jointIndex);
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
            _buildRecursive(bone, firstChildJointInfo, jointInfo->firstChildIndex);
        }

        if(jointInfo->nextSiblingIndex > 0)
        {
            if(jointInfo->nextSiblingIndex >= (int32_t)mJointInfos.size())
            {
                throw od::Exception("Next sibling index in joint info out of bounds");
            }

            JointInfo *nextSiblingJointInfo = &mJointInfos[jointInfo->nextSiblingIndex];
            _buildRecursive(parent, nextSiblingJointInfo, jointInfo->nextSiblingIndex);
        }
    }

}
