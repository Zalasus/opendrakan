/*
 * SkeletonDefinition.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include <odCore/db/SkeletonDefinition.h>

#include <algorithm>

#include <glm/matrix.hpp>

#include <odCore/Panic.h>

namespace odDb
{

    SkeletonDefinition::SkeletonDefinition()
    : mFinalized(false)
    {
    }

    void SkeletonDefinition::addJointNameInfo(const std::string &name, int32_t jointIndex)
    {
        JointNameInfo info;
        info.name = name;
        info.jointIndex = jointIndex;

        mNameInfos.push_back(info);
    }

    void SkeletonDefinition::addJointInfo(const glm::mat4 &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex)
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

    void SkeletonDefinition::reserveChannels(size_t count)
    {
        mChannelToJointIndex.resize(count, -1);
    }

    void SkeletonDefinition::markJointAsChannel(size_t jointIndex, size_t channelIndex)
    {
        if(jointIndex >= mJointInfos.size())
        {
            OD_PANIC() << "Channel index in skeleton out of joint index range: index=" << jointIndex << " size=" << mJointInfos.size();
        }

        mJointInfos[jointIndex].isChannel = true;

        if(channelIndex <= mChannelToJointIndex.size())
        {
            mChannelToJointIndex.resize(channelIndex+1, -1);
        }
        mChannelToJointIndex[channelIndex] = jointIndex;
    }

    void SkeletonDefinition::finalize()
    {
        if(!mFinalized)
        {
            for(auto &nameInfo : mNameInfos)
            {
                if(nameInfo.jointIndex < 0)
                {
                    continue;

                }else if(nameInfo.jointIndex >= (int32_t)mJointInfos.size())
                {
                    OD_PANIC() << "Joint name info's referenced joint index is out of bounds: index=" << nameInfo.jointIndex << " size=" << mJointInfos.size();
                }

                mJointInfos[nameInfo.jointIndex].nameInfo = &nameInfo;
            }

            mFinalized = true;
        }
    }

    std::optional<size_t> SkeletonDefinition::getJointIndexForChannelIndex(size_t channelIndex) const
    {
        _checkFinalized();

        if(channelIndex >= mChannelToJointIndex.size())
        {
            return {};

        }else
        {
            return mChannelToJointIndex[channelIndex];
        }
    }

    std::optional<std::string_view> SkeletonDefinition::getChannelName(size_t channelIndex) const
    {
        _checkFinalized();

        auto joint = getJointIndexForChannelIndex(channelIndex);
        if(joint.has_value())
        {
            auto nameInfo = mJointInfos[*joint].nameInfo;
            if(nameInfo == nullptr)
            {
                return {};

            }else
            {
                return nameInfo->name;
            }

        }else
        {
            return {};
        }
    }

    void SkeletonDefinition::build(odAnim::Skeleton &skeleton)
    {
        _checkFinalized();

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
            OD_PANIC() << "SkeletonDefinition fucked up and built skeleton with loops";
        }
    }

    void SkeletonDefinition::_checkFinalized() const
    {
        if(!mFinalized)
        {
            OD_PANIC() << "SkeletonDefinition not finalized before use";
        }
    }

    void SkeletonDefinition::_buildRecursive(odAnim::Skeleton &skeleton, odAnim::Skeleton::Bone *parent, JointInfo &jointInfo, int32_t jointIndex)
    {
        if(jointInfo.visited)
        {
            OD_PANIC() << "SkeletonDefinition encountered loop in bone tree";

        }else
        {
            jointInfo.visited = true;
        }

        odAnim::Skeleton::Bone &bone = (parent == nullptr) ? skeleton.addRootBone(jointIndex) : parent->addChildBone(jointIndex);
        if(jointInfo.firstChildIndex > 0)
        {
            if(jointInfo.firstChildIndex >= (int32_t)mJointInfos.size())
            {
                OD_PANIC() << "First child index in joint info out of bounds";
            }

            JointInfo &firstChildJointInfo = mJointInfos[jointInfo.firstChildIndex];
            _buildRecursive(skeleton, &bone, firstChildJointInfo, jointInfo.firstChildIndex);
        }

        if(jointInfo.nextSiblingIndex > 0)
        {
            if(jointInfo.nextSiblingIndex >= (int32_t)mJointInfos.size())
            {
                OD_PANIC() << "Next sibling index in joint info out of bounds";
            }

            JointInfo &nextSiblingJointInfo = mJointInfos[jointInfo.nextSiblingIndex];
            _buildRecursive(skeleton, parent, nextSiblingJointInfo, jointInfo.nextSiblingIndex);
        }
    }

}
