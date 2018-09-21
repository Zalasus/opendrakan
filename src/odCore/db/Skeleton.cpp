/*
 * Skeleton.cpp
 *
 *  Created on: 14 Mar 2018
 *      Author: zal
 */

#include <odCore/db/Skeleton.h>

#include <algorithm>
#include <iomanip>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Depth>

#include <odCore/Exception.h>
#include <odCore/OdDefines.h>

namespace od
{

	class BoneInfoVisitor : public osg::NodeVisitor
	{
	public:

		BoneInfoVisitor(std::ostream &out)
		: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
		, mOut(out)
		, mDepth(1)
		, mRootEncountered(false)
		{
		}

		virtual void apply(osg::Group &node)
		{
			if(!mRootEncountered)
			{
				mOut << "[ROOT]" << std::endl;
				mRootEncountered = true;
			}
			traverse(node);
		}

		virtual void apply(osg::MatrixTransform &node)
        {
			BoneNode *bn = dynamic_cast<BoneNode*>(&node);
			if(bn != nullptr)
			{
				for(size_t i = 0; i < mDepth; ++i)
				{
					mOut << "  |";
				}

				mOut << "-" << bn->getName()
					 << " (ji=" << bn->getJointInfoIndex() << " wc=" << bn->getWeightCount() << ")"
					 << std::endl;
			}

			++mDepth;
			traverse(node);
			--mDepth;
        }


	private:

		std::ostream &mOut;
		size_t mDepth;
		bool mRootEncountered;

	};



	BoneNode::BoneNode()
	: BoneNode("", -9000)
	{
	}

    BoneNode::BoneNode(const std::string &name, int32_t jointInfoIndex)
    : mJointInfoIndex(jointInfoIndex)
    , mIsChannel(false)
    , mIsRoot(false)
    , mWeightCount(0)
    {
        this->setName(name);
    }

    BoneNode::BoneNode(const BoneNode &bn, const osg::CopyOp &copyop)
    : osg::MatrixTransform(bn, copyop)
    , mJointInfoIndex(bn.mJointInfoIndex)
    , mInverseBindPoseXform(bn.mInverseBindPoseXform)
    , mIsChannel(bn.mIsChannel)
    , mIsRoot(bn.mIsRoot)
    , mWeightCount(bn.mWeightCount)
    {
    }

    void BoneNode::setInverseBindPoseXform(const osg::Matrixf &m)
    {
        mInverseBindPoseXform = m;
        _matrix.invert(m);
    }


	SkeletonBuilder::SkeletonBuilder()
	: mJointLinksDirty(true)
	{
	}

	void SkeletonBuilder::addBoneNode(const std::string &name, int32_t jointInfoIndex)
	{
	    osg::ref_ptr<BoneNode> newBoneNode(new BoneNode(name, jointInfoIndex));
	    mBoneNodes.push_back(newBoneNode);

	    mJointLinksDirty = true;
	}

	void SkeletonBuilder::addJointInfo(osg::Matrixf &boneXform, int32_t meshIndex, int32_t firstChildIndex, int32_t nextSiblingIndex)
	{
		if(mJointInfos.size() >= OD_MAX_BONE_COUNT)
		{
			throw Exception("Can't add more bones. Maximum bone count reached");
		}

		SkeletonJointInfo jointInfo;
		jointInfo.boneXform = boneXform;
		jointInfo.meshIndex = meshIndex;
		jointInfo.firstChildIndex = firstChildIndex;
		jointInfo.nextSiblingIndex = nextSiblingIndex;
		jointInfo.referencingBone = nullptr;
		jointInfo.visited = false;
		mJointInfos.push_back(jointInfo);

		mJointLinksDirty = true;
	}

	void SkeletonBuilder::makeChannel(uint32_t jointIndex)
	{
		_rebuildJointLinks();

		if(jointIndex >= mJointInfos.size())
		{
			Logger::error() << "Channel joint index out of bounds: was " << jointIndex << ", joint count is " << mJointInfos.size();
			throw Exception("Channel joint index out of bounds");
			return;
		}

		SkeletonJointInfo &jointInfo = mJointInfos[jointIndex];

		if(jointInfo.referencingBone == nullptr)
		{
			throw Exception("Tried to turn unreferenced joint into channel");
		}

		jointInfo.referencingBone->setIsChannel(true);
	}

	void SkeletonBuilder::build(osg::Group *rootNode)
	{
		// first, create links between nodes and joint infos if any of those changed
		_rebuildJointLinks();

		// then, reset all visited flags in case we already built a skeleton
		for(auto it = mJointInfos.begin(); it != mJointInfos.end(); ++it)
		{
			it->visited = false;

			// draw all joints as balls for testing
			/*osg::ref_ptr<osg::MatrixTransform> tf = new osg::MatrixTransform();
			tf->setMatrix(osg::Matrix::inverse(it->boneXform));
			osg::ref_ptr<osg::Geode> cylinder = new osg::Geode();
			osg::ref_ptr<osg::Sphere> cylinderShape = new osg::Sphere(osg::Vec3(0,0,0), 0.005);
			osg::ref_ptr<osg::ShapeDrawable> cylinderDrawable = new osg::ShapeDrawable(cylinderShape);
			cylinderDrawable->setColor(osg::Vec4(1, 0, 0, 1));
			cylinder->addDrawable(cylinderDrawable);
			cylinder->getOrCreateStateSet()->setRenderBinDetails(5, "RenderBin");
			cylinder->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS), osg::StateAttribute::ON);
			cylinder->getOrCreateStateSet()->setAttribute(new osg::Program);
			tf->addChild(cylinder);
			rootNode->addChild(tf);*/
		}

		if(rootNode != nullptr)
		{
			// for simplicity, recurse through tree depth first for all joints and build it inside the scenegraph
			for(auto it = mJointInfos.begin(); it != mJointInfos.end(); ++it)
			{
				if(it->referencingBone == nullptr)
				{
					continue;
				}

				if(!it->visited)
				{
					_buildRecursive(*rootNode, *it);
				}
			}
		}
	}

	void SkeletonBuilder::printInfo(std::ostream &out)
	{
		osg::ref_ptr<osg::Group> root(new osg::Group);
		this->build(root);

	    BoneInfoVisitor biv(out);
	    root->accept(biv);
	}

	void SkeletonBuilder::_rebuildJointLinks()
	{
		if(mJointLinksDirty)
		{
			for(auto it = mBoneNodes.begin(); it != mBoneNodes.end(); ++it)
			{
				BoneNode *currentBone = it->get();
				int32_t jointInfoIndex = currentBone->getJointInfoIndex();
				if(jointInfoIndex < 0)
				{
					continue;

				}else if(jointInfoIndex >= (int32_t)mJointInfos.size())
				{
					throw Exception("Node's joint info index out of bounds");
				}

				SkeletonJointInfo &jointInfo = mJointInfos[jointInfoIndex];

				currentBone->setInverseBindPoseXform(jointInfo.boneXform);
				currentBone->setWeightCount(jointInfo.weightCount);
				jointInfo.referencingBone = currentBone;
			}

			mJointLinksDirty = false;
		}
	}

	void SkeletonBuilder::_buildRecursive(osg::Group &parent, SkeletonJointInfo &current)
	{
		if(current.visited)
		{
			throw Exception("Bone tree is not a tree");

		}else
		{
			current.visited = true;
		}

		osg::ref_ptr<BoneNode> newChild(new BoneNode(*current.referencingBone, osg::CopyOp::SHALLOW_COPY));
		parent.addChild(newChild);

		if(current.firstChildIndex > 0)
		{
			if(current.firstChildIndex >= (int32_t)mJointInfos.size())
			{
				throw Exception("First child index in joint info out of bounds");
			}

			SkeletonJointInfo &firstChildJointInfo = mJointInfos[current.firstChildIndex];
			if(firstChildJointInfo.referencingBone == nullptr)
			{
				throw Exception("First child joint info is unreferenced joint");
			}

			_buildRecursive(*newChild, firstChildJointInfo);
		}

		if(current.nextSiblingIndex > 0)
		{
			if(current.nextSiblingIndex >= (int32_t)mJointInfos.size())
			{
				throw Exception("Next sibling index in joint info out of bounds");
			}

			SkeletonJointInfo &nextSiblingJointInfo = mJointInfos[current.nextSiblingIndex];
			if(nextSiblingJointInfo.referencingBone == nullptr)
			{
				throw Exception("Next sibling joint info is unreferenced joint");
			}

			_buildRecursive(parent, nextSiblingJointInfo);
		}
	}
}
