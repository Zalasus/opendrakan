/*
 * BoneAnimator.h
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_ANIM_BONEANIMATOR_H_
#define INCLUDE_ANIM_BONEANIMATOR_H_

#include <osg/NodeCallback>

#include "anim/Animator.h"

namespace od
{

	/**
	 * Extension of Animator that uploads bone transforms to the rigging shader.
	 */
	class BoneAnimator : public Animator
	{
	public:

		BoneAnimator(BoneNode *boneNode, osg::Uniform *boneMatrixArray);
		~BoneAnimator();

		inline BoneNode *getBoneNode() { return mBoneNode; }


	private:

		osg::ref_ptr<BoneNode> mBoneNode; // redundant, but saves us an ugly cast
		osg::ref_ptr<osg::NodeCallback> mUploadCallback;

	};

}

#endif /* INCLUDE_ANIM_BONEANIMATOR_H_ */
