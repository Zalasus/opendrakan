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

namespace odDb
{


    class Skeleton
    {
    public:

        void addNode(const std::string &name, int32_t jointInfoIndex);
        void addJointInfo(const glm::mat4 &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
        void markJointAsChannel(int32_t jointIndex);


    private:

        struct NodeInfo
        {
            std::string name;
            int32_t jointInfoIndex;
        };

        struct JointInfo
        {
            glm::mat4 boneXform;
            int32_t meshIndex;
            int32_t firstChildIndex;
            int32_t nextSiblingIndex;
            bool isChannel;
        };

        std::vector<NodeInfo> mNodes;
        std::vector<JointInfo> mJoints;
    };

}

#endif /* INCLUDE_DB_SKELETON_H_ */
