/*
 * Skeleton.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include <odCore/anim/Skeleton.h>

#include <algorithm>

#include <glm/matrix.hpp>

#include <odCore/Exception.h>

#include <odCore/render/Rig.h>

namespace odAnim
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
}
