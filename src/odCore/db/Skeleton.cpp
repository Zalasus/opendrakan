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

    void Skeleton::addNode(const std::string &name, int32_t jointInfoIndex)
    {
        NodeInfo info;
        info.name = name;
        info.jointInfoIndex = jointInfoIndex;

        mNodes.push_back(info);
    }

    void Skeleton::addJointInfo(const glm::mat4 &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex)
    {
        JointInfo info;
        info.boneXform = boneXform;
        info.meshIndex = meshIndex;
        info.firstChildIndex = firstChildIndex;
        info.nextSiblingIndex = nextSiblingIndex;

        mJoints.push_back(info);
    }

    void Skeleton::markJointAsChannel(int32_t jointIndex)
    {
        if(jointIndex < 0 || jointIndex >= mJoints.size())
        {
            throw od::Exception("Channel index in skeleton out of joint index range");
        }

        mJoints[jointIndex].isChannel = true;
    }

}
