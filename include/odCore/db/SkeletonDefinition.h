/*
 * SkeletonDefinition.h
 *
 *  Created on: 11 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_SKELETONDEFINITION_H_
#define INCLUDE_DB_SKELETONDEFINITION_H_

#include <string>
#include <vector>
#include <optional>

#include <glm/mat4x4.hpp>
#include <glm/mat3x4.hpp>

#include <odCore/anim/Skeleton.h>

namespace odDb
{

    class SkeletonDefinition
    {
    public:

        SkeletonDefinition();
        SkeletonDefinition(const SkeletonDefinition &sd) = delete;

        inline size_t getJointCount() const { return mJointInfos.size(); }

        void addJointNameInfo(const std::string &name, int32_t jointIndex);
        void addJointInfo(const glm::mat4 &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
        void reserveChannels(size_t count);
        void markJointAsChannel(size_t jointIndex, size_t channelIndex);
        void finalize();

        std::optional<size_t> getJointIndexForChannelIndex(size_t channelIndex) const;
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

        void _checkFinalized() const;
        void _buildRecursive(odAnim::Skeleton &skeleton, odAnim::Skeleton::Bone *parent, JointInfo &jointInfo, int32_t jointIndex);

        std::vector<JointNameInfo> mNameInfos;
        std::vector<JointInfo> mJointInfos;
        std::vector<int32_t> mChannelToJointIndex; // index i contains the joint index corresponding to channel i
        bool mFinalized;
    };

}

#endif /* INCLUDE_DB_SKELETON_H_ */
