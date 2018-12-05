/*
 * SkeletonBuilder.h
 *
 *  Created on: 11 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_SKELETONBUILDER_H_
#define INCLUDE_DB_SKELETONBUILDER_H_

#include <string>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/mat3x4.hpp>

#include <odCore/anim/Skeleton.h>

namespace odRender
{
    class Rig;
}

namespace odDb
{

    class SkeletonBuilder
    {
    public:

        SkeletonBuilder();
        SkeletonBuilder(SkeletonBuilder &sb) = delete;

        inline size_t getJointCount() const { return mJointInfos.size(); }

        void addJointNameInfo(const std::string &name, int32_t jointIndex);
        void addJointInfo(const glm::mat4 &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
        void markJointAsChannel(size_t jointIndex);

        void build(odAnim::Skeleton &skeleton);


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

        void _buildRecursive(odAnim::Skeleton &skeleton, odAnim::Skeleton::Bone *parent, JointInfo &jointInfo, int32_t jointIndex);

        std::vector<JointNameInfo> mNameInfos;
        std::vector<JointInfo> mJointInfos;
        bool mAlreadyBuiltNameLinks;
    };

}

#endif /* INCLUDE_DB_SKELETON_H_ */
