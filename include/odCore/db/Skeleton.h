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
#include <ostream>
#include <glm/mat4x4.hpp>

namespace odDb
{

    /**
     * Class representing a bone in a skeleton as a MatrixTransform. This way we can use the same
     * method to animate bones as we use to move objects around in an interpolated manner.
     */
    class BoneNode
    {
    public:

    	BoneNode();
        BoneNode(const std::string &name, int32_t jointInfoIndex);
        BoneNode(const BoneNode &bn);

        inline int32_t getJointInfoIndex() const { return mJointInfoIndex; }
        inline size_t getWeightCount() const { return mWeightCount; }
        inline void setWeightCount(size_t w) { mWeightCount = w; }
        inline bool isChannel() const { return mIsChannel; }
        inline void setIsChannel(bool b) { mIsChannel = b; }
        inline bool isRoot() const { return mJointInfoIndex == 0; } // TODO: replace with better condition as we can have multiple roots in theory

        inline const glm::mat4 &getInverseBindPoseXform() const { return mInverseBindPoseXform; }
        void setInverseBindPoseXform(const glm::mat4 &m);


    private:

        int32_t mJointInfoIndex;
        glm::mat4 mInverseBindPoseXform;
        bool mIsChannel;
        bool mIsRoot;
        size_t mWeightCount;
    };

	class SkeletonBuilder
	{
	public:

		SkeletonBuilder();

		void addBoneNode(const std::string &name, int32_t jointInfoIndex);
		void addJointInfo(glm::mat4 &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
		void makeChannel(uint32_t jointIndex);
		void build();
		void printInfo(std::ostream &out);

		void pfffSetWeightCount(size_t wc) { mJointInfos.back().weightCount = wc; }

	private:

		struct SkeletonJointInfo
        {
            glm::mat4 boneXform;
            int32_t meshIndex;
            int32_t firstChildIndex;
            int32_t nextSiblingIndex;

            bool visited;
            BoneNode *referencingBone;

            size_t weightCount;
        };

		void _rebuildJointLinks();
		void _buildRecursive(BoneNode &parent, SkeletonJointInfo &current);

		std::vector<<BoneNode>> mBoneNodes;
		std::vector<SkeletonJointInfo> mJointInfos;
		bool mJointLinksDirty;
	};

}

#endif /* INCLUDE_DB_SKELETON_H_ */
