/*
 * SkeletonAnimation.h
 *
 *  Created on: 25 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_ANIM_SKELETONANIMATION_H_
#define INCLUDE_ANIM_SKELETONANIMATION_H_

#include <vector>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/Program>
#include <osg/Uniform>

#include "anim/Animation.h"

namespace od
{

	class Engine;

	/**
	 * Extension of Animation allowing to load animations from riot databases and distributing their keyframes
	 * among the Animation's Animator objects. This also manages loading the rigging shader and uploading
	 * the bone matrices to the GPU.
	 */
	class SkeletonAnimation : public Animation
	{
	public:

		SkeletonAnimation(Engine &engine, osg::Node *modelNode, osg::Group *skeletonRoot); // we need an engine instance to load the rigging shader
		~SkeletonAnimation();

		inline osg::Uniform *getBoneMatrixListUniform() { return mBoneMatrixList.get(); }


	private:

		Engine &mEngine;
		osg::ref_ptr<osg::Node> mModelNode;
		osg::ref_ptr<osg::Group> mSkeletonRoot;
		osg::ref_ptr<osg::Uniform> mBoneMatrixList;
		std::vector<osg::ref_ptr<Animator>> mAnimators;
		osg::ref_ptr<osg::Program> mRiggingProgram;
	};

}

#endif /* INCLUDE_ANIM_SKELETONANIMATION_H_ */
