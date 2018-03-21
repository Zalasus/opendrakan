/*
 * Skeleton.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include "db/Skeleton.h"

#include <algorithm>
#include <iomanip>

#include "Exception.h"

namespace od
{

	SkeletonNode::SkeletonNode(const std::string &name, int32_t jointInfoIndex)
	: mName(name)
	, mJointInfoIndex(jointInfoIndex)
	, mReferencedJointInfo(nullptr)
	, mIsChannel(false)
	{
	}

	void SkeletonNode::print(std::ostream &out, size_t depth)
	{
		for(size_t i = 0; i < depth; ++i)
		{
			out << "   |";

			if(i + 1 == depth)
			{
				out << "-";
			}
		}

		if(mJointInfoIndex < 0)
		{
			out << "(" << getName() << ")" << std::endl;

		}else
		{
			out << getName();
			if(mIsChannel)
			{
				out << " [Channel]";
			}
			out << std::endl;
		}

		for(auto it = mChildren.begin(); it != mChildren.end(); ++it)
		{
			if(*it == nullptr)
			{
				continue;
			}

			(*it)->print(out, depth+1);
		}
	}


	SkeletonBuilder::SkeletonBuilder(const std::string &modelName)
	: mModelName(modelName)
	, mRootNode("[ROOT]", -9000)
	, mLastJoint(nullptr)
	{
	}

	void SkeletonBuilder::reserveNodes(size_t nodeCount)
	{
		if(nodeCount > 0)
		{
			mNodes.reserve(nodeCount);
		}
	}

	void SkeletonBuilder::addNode(const std::string &name, int32_t jointInfoIndex)
	{
		mNodes.push_back(SkeletonNode(name, jointInfoIndex));

		if(jointInfoIndex >= 0)
		{
			mLastJoint = &mNodes.back();

		}else
		{
			if(mLastJoint == nullptr)
			{
				throw Exception("Found non-joint child before any actual joint node");
			}

			mLastJoint->addChild(mNodes.back());
		}
	}

	void SkeletonBuilder::addJointInfo(osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex)
	{
		SkeletonJointInfo jointInfo;
		jointInfo.boneXform = boneXform;
		jointInfo.meshIndex = meshIndex;
		jointInfo.firstChildIndex = firstChildIndex;
		jointInfo.nextSiblingIndex = nextSiblingIndex;
		jointInfo.referencingNode = nullptr;
		jointInfo.visited = false;
		mJointInfos.push_back(jointInfo);
	}

	void SkeletonBuilder::makeChannel(uint32_t jointIndex)
	{
		if(jointIndex >= mJointInfos.size())
		{
			Logger::warn() << "Channel joint index out of bounds: was " << jointIndex << ", joint count is " << mJointInfos.size();
			//throw Exception("Channel joint index out of bounds");
			return;
		}

		SkeletonJointInfo &jointInfo = mJointInfos[jointIndex];

		if(jointInfo.referencingNode == nullptr)
		{
			throw Exception("Tried to turn unreferenced joint into channel. May need to build skeleton first before defining channels");
		}

		jointInfo.referencingNode->mIsChannel = true;
	}

	void SkeletonBuilder::build()
	{
		// first, create links between nodes and joint infos
		for(auto it = mNodes.begin(); it != mNodes.end(); ++it)
		{
			int32_t jointInfoIndex = it->getJointInfoIndex();
			if(jointInfoIndex < 0)
			{
				continue;

			}else if(jointInfoIndex >= (int32_t)mJointInfos.size())
			{
				throw Exception("Node's joint info index out of bounds");
			}
			SkeletonJointInfo &jointInfo = mJointInfos[jointInfoIndex];

			it->mReferencedJointInfo = &jointInfo;
			jointInfo.referencingNode = &(*it);
		}

		// for testing purposes, recurse through tree depth first for all joints while building the tree with references, then print it out
		for(auto it = mNodes.begin(); it != mNodes.end(); ++it)
		{
			if(it->mReferencedJointInfo == nullptr)
			{
				continue;
			}

			if(!it->mReferencedJointInfo->visited)
			{
				_buildRecursive(mRootNode, *it);
			}
		}
	}

	void SkeletonBuilder::printInfo(std::ostream &out)
	{
		out << "Skeleton info for model " << mModelName << std::endl;

		out << std::endl << "Nodes:" << std::endl;
		for(size_t i = 0; i < mNodes.size(); ++i)
		{
			SkeletonNode &currentNode = mNodes[i];

			out << std::setw(3) << i << ": "
				<< std::setw(16) << (std::string("'") + currentNode.getName() + "'")
				<< " jointInfo=" << std::setw(4) << currentNode.getJointInfoIndex() << std::endl;
		}

		out << std::endl << "Joint info:" << std::endl;
		for(size_t i = 0; i < mJointInfos.size(); ++i)
		{
			SkeletonJointInfo &jointInfo = mJointInfos[i];

			out << std::setw(3) << i << ": "
				<< "meshIndex=" << std::setw(4) << jointInfo.meshIndex << " "
				<< "firstChild=" << std::setw(4) << jointInfo.firstChildIndex << " "
				<< "nextSibling=" << std::setw(4) << jointInfo.nextSiblingIndex << std::endl;
		}

		out << std::endl << "Constructed bone tree: " << std::endl;
		mRootNode.print(out, 0);
	}

	void SkeletonBuilder::_buildRecursive(SkeletonNode &parent, SkeletonNode &current)
	{
		if(current.mReferencedJointInfo->visited)
		{
			throw Exception("Bone tree is not a tree");

		}else
		{
			current.mReferencedJointInfo->visited = true;
		}

		parent.addChild(current);

		if(current.mReferencedJointInfo->firstChildIndex > 0)
		{
			if(current.mReferencedJointInfo->firstChildIndex >= (int32_t)mJointInfos.size())
			{
				throw Exception("First child index in joint info out of bounds");
			}

			SkeletonJointInfo &firstChildJointInfo = mJointInfos[current.mReferencedJointInfo->firstChildIndex];
			if(firstChildJointInfo.referencingNode == nullptr)
			{
				throw Exception("First child joint info is unreferenced joint");
			}

			_buildRecursive(current, *firstChildJointInfo.referencingNode);
		}

		if(current.mReferencedJointInfo->nextSiblingIndex > 0)
		{
			if(current.mReferencedJointInfo->nextSiblingIndex >= (int32_t)mJointInfos.size())
			{
				throw Exception("Next sibling index in joint info out of bounds");
			}

			SkeletonJointInfo &nextSiblingJointInfo = mJointInfos[current.mReferencedJointInfo->nextSiblingIndex];
			if(nextSiblingJointInfo.referencingNode == nullptr)
			{
				throw Exception("Next sibling joint info is unreferenced joint");
			}

			_buildRecursive(parent, *nextSiblingJointInfo.referencingNode);
		}
	}


}
