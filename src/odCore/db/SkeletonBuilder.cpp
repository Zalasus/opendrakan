/*
 * SkeletonBuilder.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include <odCore/db/SkeletonBuilder.h>

#include <algorithm>

#include <glm/matrix.hpp>

#include <odCore/Exception.h>

namespace odDb
{

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
        info.visited = false;
        info.nameInfo = nullptr;

        mJointInfos.push_back(info);
    }

    void SkeletonBuilder::reserveChannels(size_t count)
    {
        mChannelToJointIndex.resize(count, -1);
    }

    void SkeletonBuilder::markJointAsChannel(size_t jointIndex, size_t channelIndex)
    {
        if(jointIndex >= mJointInfos.size())
        {
            throw od::Exception("Channel index in skeleton out of joint index range");
        }

        mJointInfos[jointIndex].isChannel = true;

        if(channelIndex <= mChannelToJointIndex.size())
        {
            mChannelToJointIndex.resize(channelIndex+1, -1);
        }
        mChannelToJointIndex[channelIndex] = jointIndex;
    }

    void SkeletonBuilder::build(odAnim::Skeleton &skeleton)
    {
        if(!mAlreadyBuiltNameLinks)
        {
            for(auto &nameInfo : mNameInfos)
            {
                if(nameInfo.jointIndex < 0)
                {
                    continue;

                }else if(nameInfo.jointIndex >= (int32_t)mJointInfos.size())
                {
                    throw od::Exception("Joint name info's referenced joint index is out of bounds");
                }

                mJointInfos[nameInfo.jointIndex].nameInfo = &nameInfo;
            }

            mAlreadyBuiltNameLinks = true;
        }

        for(auto &jointInfo : mJointInfos)
        {
            jointInfo.visited = false;
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
            _buildRecursive(skeleton, nullptr, *it, jointIndex);
        }

        if(skeleton.checkForLoops())
        {
            throw od::Exception("SkeletonBuilder fucked up and built skeleton with loops");
        }
    }

    void SkeletonBuilder::_buildRecursive(odAnim::Skeleton &skeleton, odAnim::Skeleton::Bone *parent, JointInfo &jointInfo, int32_t jointIndex)
    {
        if(jointInfo.visited)
        {
            throw od::Exception("SkeletonBuilder encountered loop in bone tree");

        }else
        {
            jointInfo.visited = true;
        }

        odAnim::Skeleton::Bone &bone = (parent == nullptr) ? skeleton.addRootBone(jointIndex) : parent->addChildBone(jointIndex);
        bone.setInverseBindPoseTransform(jointInfo.boneXform);
        if(jointInfo.nameInfo != nullptr)
        {
            bone.setName(jointInfo.nameInfo->name);
        }

        if(jointInfo.firstChildIndex > 0)
        {
            if(jointInfo.firstChildIndex >= (int32_t)mJointInfos.size())
            {
                throw od::Exception("First child index in joint info out of bounds");
            }

            JointInfo &firstChildJointInfo = mJointInfos[jointInfo.firstChildIndex];
            _buildRecursive(skeleton, &bone, firstChildJointInfo, jointInfo.firstChildIndex);
        }

        if(jointInfo.nextSiblingIndex > 0)
        {
            if(jointInfo.nextSiblingIndex >= (int32_t)mJointInfos.size())
            {
                throw od::Exception("Next sibling index in joint info out of bounds");
            }

            JointInfo &nextSiblingJointInfo = mJointInfos[jointInfo.nextSiblingIndex];
            _buildRecursive(skeleton, parent, nextSiblingJointInfo, jointInfo.nextSiblingIndex);
        }
    }

}
