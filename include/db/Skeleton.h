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
#include <osg/Matrixf>
#include <osg/Referenced>


namespace od
{
	class SkeletonNode;

	struct SkeletonJointInfo
	{
		osg::Matrixf boneXform;
		int32_t meshIndex;
		int32_t firstChildIndex;
		int32_t nextSiblingIndex;

		bool visited;
		SkeletonNode *referencingNode;
	};

	class SkeletonNode
	{
	public:

		friend class SkeletonBuilder;

		SkeletonNode(const std::string &name, int32_t jointInfoIndex);

		inline int32_t getJointInfoIndex() { return mJointInfoIndex; }
		inline std::string getName() { return mName; }

		void addChild(SkeletonNode &node) { mChildren.push_back(&node); }
		void print(std::ostream &out, size_t depth);


	private:

		std::string mName;
		int32_t mJointInfoIndex;
		std::vector<SkeletonNode*> mChildren;
		SkeletonJointInfo *mReferencedJointInfo;
		bool mIsChannel;
	};

	class SkeletonBuilder
	{
	public:

		SkeletonBuilder(const std::string &modelName);

		void reserveNodes(size_t nodeCount);
		void addNode(const std::string &name, int32_t jointInfoIndex);
		void addJointInfo(osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex);
		void makeChannel(uint32_t jointIndex);
		void build();
		void printInfo(std::ostream &out);


	private:

		void _buildRecursive(SkeletonNode &parent, SkeletonNode &current);

		std::string mModelName;
		SkeletonNode mRootNode;
		std::vector<SkeletonNode> mNodes;
		std::vector<SkeletonJointInfo> mJointInfos;
		SkeletonNode *mLastJoint;
	};

}

#endif /* INCLUDE_DB_SKELETON_H_ */
