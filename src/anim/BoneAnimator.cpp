/*
 * BoneAnimator.cpp
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#include "anim/BoneAnimator.h"

namespace od
{
	class UploadBoneCallback : public osg::NodeCallback
	{
	public:

		UploadBoneCallback(BoneNode *bn, osg::Uniform *boneMatrixArray)
		: mBoneNode(bn)
		, mBoneMatrixArray(boneMatrixArray)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			if(node != mBoneNode)
			{
				return;
			}

			mBoneMatrixArray->setElement(mBoneNode->getJointInfoIndex(), mBoneNode->getMatrix() * mBoneNode->getInverseBindPoseXform());

			traverse(node, nv);
		}


	private:

		osg::ref_ptr<BoneNode> mBoneNode;
		osg::ref_ptr<osg::Uniform> mBoneMatrixArray;
	};





	BoneAnimator::BoneAnimator(BoneNode *boneNode, osg::Uniform *boneMatrixArray)
	: Animator(boneNode)
	, mBoneNode(boneNode)
	, mUploadCallback(new UploadBoneCallback(boneNode, boneMatrixArray))
	{
		mBoneNode->addUpdateCallback(mUploadCallback);
	}

	BoneAnimator::~BoneAnimator()
	{
		mBoneNode->removeUpdateCallback(mUploadCallback);
	}


}
