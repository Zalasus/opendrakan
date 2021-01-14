/*
 * Skeleton.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include <odCore/anim/Skeleton.h>

#include <algorithm>

#include <glm/matrix.hpp>

#include <odCore/Panic.h>

#include <odCore/db/SkeletonDefinition.h>

#include <odCore/render/Rig.h>

namespace odAnim
{

    Skeleton::Bone::Bone(Skeleton &skeleton, int32_t jointIndex)
    : mSkeleton(skeleton)
    , mParent(nullptr)
    , mJointIndex(jointIndex)
    {
    }

    size_t Skeleton::Bone::getChildBoneCount()
    {
        return mChildBones.size();
    }

    Skeleton::Bone &Skeleton::Bone::getChildBone(size_t index)
    {
        if(index >= mChildBones.size())
        {
            OD_PANIC() << "Child bone index out of bounds: index=" << index << " size=" << mChildBones.size();
        }

        return *mChildBones[index];
    }

    Skeleton::Bone &Skeleton::Bone::addChildBone(int32_t jointIndex)
    {
        if(jointIndex < 0 || (size_t)jointIndex >= mSkeleton.mBones.size())
        {
            OD_PANIC() << "Child bone joint index passed to bone out of bounds: index=" << jointIndex << " size=" << mSkeleton.mBones.size();
        }

        Bone &childBone = mSkeleton.mBones[jointIndex];

        if(&childBone == this)
        {
            OD_PANIC() << "Tried to add bone to itself as a child";
        }

        childBone.mParent = this;
        mChildBones.push_back(&childBone);

        return childBone;
    }

    void Skeleton::Bone::moveToBindPose()
    {
        mCurrentMatrix = glm::mat4(1.0);
    }

    void Skeleton::Bone::move(const glm::mat4 &transform)
    {
        mCurrentMatrix = transform;
    }

    void Skeleton::Bone::_flattenRecursive(odRender::Rig &rig, const glm::mat4 &parentMatrix)
    {
        glm::mat4 chainMatrix = mCurrentMatrix * parentMatrix;

        rig.setBoneTransform(mJointIndex, chainMatrix);

        for(auto it = mChildBones.begin(); it != mChildBones.end(); ++it)
        {
            (*it)->_flattenRecursive(rig, chainMatrix);
        }
    }


    Skeleton::Skeleton(std::shared_ptr<odDb::SkeletonDefinition> def)
    : mDefinition(def)
    {
        size_t boneCount = mDefinition->getJointCount();
        mBones.reserve(boneCount);
        for(size_t i = 0; i < boneCount; ++i)
        {
            mBones.emplace_back(*this, i);
        }

        mDefinition->build(*this);
    }

    Skeleton::Bone &Skeleton::addRootBone(int32_t jointIndex)
    {
        if(jointIndex < 0 || (size_t)jointIndex >= mBones.size())
        {
            OD_PANIC() << "Root bone joint index passed to skeleton out of bounds: index=" << jointIndex << " size=" << mBones.size();
        }

        Bone &rootBone = mBones[jointIndex];
        rootBone.mParent = nullptr;
        mRootBones.push_back(&rootBone);

        return rootBone;
    }

    Skeleton::Bone &Skeleton::getBoneByJointIndex(int32_t jointIndex)
    {
        if(jointIndex < 0 || (size_t)jointIndex >= mBones.size())
        {
            OD_PANIC() << "Joint index passed to skeleton out of bounds: index=" << jointIndex << " size=" << mBones.size();
        }

        return mBones[jointIndex];
    }

    Skeleton::Bone &Skeleton::getBoneByChannelIndex(int32_t channelIndex)
    {
        return mBones[mDefinition->getJointIndexForChannelIndex(channelIndex).value()];
    }

    void Skeleton::flatten(odRender::Rig &rig)
    {
        glm::mat4 eye(1.0);

        for(auto root : mRootBones)
        {
            root->_flattenRecursive(rig, eye);
        }
    }

    bool Skeleton::checkForLoops()
    {
        std::vector<bool> visited(mBones.size(), false);
        bool hasLoop = false;
        auto f = [&visited, &hasLoop](Bone &b)
        {
            if(visited[b.getJointIndex()])
            {
                hasLoop = true;
                return false;
            }

            visited[b.getJointIndex()] = true;
            return true;
        };

        traverse(f);

        return hasLoop;
    }
}
