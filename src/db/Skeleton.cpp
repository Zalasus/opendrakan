/*
 * Skeleton.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include "db/Skeleton.h"

#include <algorithm>

#include "Exception.h"

namespace od
{

	Bone::Bone(const std::string &name, int32_t parentIndex)
	: mName(name)
	, mParent(nullptr)
	, mParentIndex(parentIndex)
	{
	}

	void Bone::setParent(Bone *parent)
	{
		// if we have a parent, remove ourself from our former parent's child list
		if(mParent != nullptr)
		{
			auto it = std::find(mParent->mChildren.begin(), mParent->mChildren.end(), this);
			if(it != mParent->mChildren.end())
			{
				mParent->mChildren.erase(it);
			}
		}

		mParent = parent;

		if(mParent != nullptr)
		{
			// we've got a new parent. let's add ourself to our parent's child list if we aren't in there already
			if(std::find(mParent->mChildren.begin(), mParent->mChildren.end(), this) == mParent->mChildren.end())
			{
				mParent->mChildren.push_back(this);
			}
		}
	}

	void Bone::setInverseBindPoseXform(const osg::Matrixf &inverseBindPoseXform)
	{
		mInverseBindPoseXform = inverseBindPoseXform;
		mCurrentXform = osg::Matrixf::inverse(inverseBindPoseXform);
	}

	void Bone::debugLogBone(size_t depth)
	{
		for(size_t i = 0; i < depth; ++i)
		{
			std::cout << "   |";

			if(i + 1 == depth)
			{
				std::cout << "-";
			}
		}
		std::cout << getName() << std::endl;

		for(auto it = mChildren.begin(); it != mChildren.end(); ++it)
		{
			if(*it == nullptr)
			{
				continue;
			}

			(*it)->debugLogBone(depth+1);
		}
	}



	Skeleton::Skeleton(size_t boneCount)
	: mRootBone("[ROOT]", -1)
	{
		if(boneCount > 0)
		{
			mBones.reserve(boneCount);
		}
	}

	Bone *Skeleton::addBone(const std::string &name, int32_t parentIndex)
	{
		osg::ref_ptr<Bone> b = new Bone(name, parentIndex);
		mBones.push_back(b);
		return b.get();
	}

	Bone *Skeleton::getRootBone()
	{
		return &mRootBone;
	}

	Bone *Skeleton::getBoneByIndex(size_t index)
	{
		if(index >= mBones.size())
		{
			throw NotFoundException("Bone index out of bounds");
			return nullptr;
		}

		return mBones[index];
	}

	void Skeleton::buildBoneTree()
	{
		for(size_t i = 0; i < mBones.size(); ++i)
		{
			Bone *b = mBones[i].get();

			if(b->getParentIndex() < 0)
			{
				mRootBone.mChildren.push_back(b);

			}else
			{
				Bone *parentBone = mBones[b->getParentIndex()];

				if(parentBone == nullptr)
				{
					throw Exception("Invalid parent index");
				}

				b->setParent(parentBone);
			}
		}
	}

	void Skeleton::debugLogSkeleton()
	{
		getRootBone()->debugLogBone(0);
		std::cout << std::endl;
	}




	BuilderBone::BuilderBone(const std::string &name, int32_t parentIndex)
	: mName(name)
	, mParentIndex(parentIndex)
	, mMeshIndex(-1)
	, mFirstChildIndex(-1)
	, mNextSiblingIndex(-1)
	, mVisited(false)
	{
	}

	void BuilderBone::setBoneData(osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex)
	{
		mInverseBindPoseXform = boneXform;
		mMeshIndex = meshIndex;
		mFirstChildIndex = firstChildIndex;
		mNextSiblingIndex = nextSiblingIndex;
	}

	void BuilderBone::print(size_t depth)
	{
		for(size_t i = 0; i < depth; ++i)
		{
			std::cout << "   |";

			if(i + 1 == depth)
			{
				std::cout << "-";
			}
		}
		std::cout << getName() << std::endl;

		for(auto it = mChildren.begin(); it != mChildren.end(); ++it)
		{
			if(*it == nullptr)
			{
				continue;
			}

			(*it)->print(depth+1);
		}
	}


	SkeletonBuilder::SkeletonBuilder(const std::string &modelName)
	: mModelName(modelName)
	{
	}

	void SkeletonBuilder::reserveBones(size_t boneCount)
	{
		if(boneCount > 0)
		{
			mBuilderBones.reserve(boneCount);
		}
	}

	void SkeletonBuilder::addBoneName(const std::string &name, int32_t parentIndex)
	{
		mBuilderBones.push_back(BuilderBone(name, parentIndex));
	}

	void SkeletonBuilder::setBoneData(size_t boneIndex, osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex)
	{
		if(boneIndex >= mBuilderBones.size())
		{
			return; // throw whatever
		}

		mBuilderBones[boneIndex].setBoneData(boneXform, meshIndex, firstChildIndex, nextSiblingIndex);
	}

	void SkeletonBuilder::build()
	{
		// for testing purposes, recurse through tree depth first while building the tree with references, then print it out

		BuilderBone root("[ROOT] " + mModelName, -9000);
		if(mBuilderBones.size() > 0)
		{
			_buildRecursive(root, mBuilderBones[0]);

			for(size_t i = 1; i < mBuilderBones.size(); ++i)
			{
				BuilderBone &currentBone = mBuilderBones[i];
				if(!currentBone.mVisited)
				{
					if(currentBone.mParentIndex >= 0)
					{
						_buildRecursive(mBuilderBones[currentBone.mParentIndex], currentBone);

					}else
					{
						_buildRecursive(root, currentBone);
					}
				}
			}
		}

		root.print(0);
	}

	// it looks elegant, but deep down you know it's bad...
	void SkeletonBuilder::_buildRecursive(BuilderBone &parent, BuilderBone &current)
	{
		if(current.mVisited)
		{
			throw Exception("Bone tree is not a tree");

		}else
		{
			current.mVisited = true;
		}

		parent.addChild(current);

		if(current.mFirstChildIndex > 0)
		{
			_buildRecursive(current, mBuilderBones[current.mFirstChildIndex]);
		}

		if(current.mNextSiblingIndex > 0)
		{
			_buildRecursive(parent, mBuilderBones[current.mNextSiblingIndex]);
		}
	}


}
