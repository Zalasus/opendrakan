/*
 * SkeletonAnimationPlayer.h
 *
 *  Created on: 25 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_ANIM_SKELETONANIMATIONPLAYER_H_
#define INCLUDE_ANIM_SKELETONANIMATIONPLAYER_H_

#include <vector>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/Program>
#include <osg/Uniform>

#include "anim/AnimationPlayer.h"
#include "anim/Animator.h"
#include "db/Animation.h"

namespace od
{

	class Engine;

	/**
	 * Extension of AnimationPlayer allowing to load animations from riot database assets and distributing their keyframes
	 * among the AnimationPlayer's Animator objects. This also manages loading the rigging shader and uploading
	 * the bone matrices to the GPU.
	 */
	class SkeletonAnimationPlayer : public AnimationPlayer
	{
	public:

		SkeletonAnimationPlayer(Engine &engine, osg::Node *modelNode, osg::Group *skeletonRoot); // we need an engine instance to load the rigging shader
		~SkeletonAnimationPlayer();

		inline osg::Uniform *getBoneMatrixArrayUniform() { return mBoneMatrixArray.get(); }


		void setAnimation(osg::ref_ptr<Animation> anim);
		void setPlayState(AnimationPlayState state);
		AnimationPlayState getPlayState();

	private:

		Engine &mEngine;
		osg::ref_ptr<osg::Node> mModelNode;
		osg::ref_ptr<osg::Group> mSkeletonRoot;
		osg::ref_ptr<osg::Uniform> mBoneMatrixArray;
		std::vector<osg::ref_ptr<Animator>> mAnimators;
		osg::ref_ptr<osg::Program> mRiggingProgram;
		osg::ref_ptr<osg::NodeCallback> mUploadCallback;

		osg::ref_ptr<Animation> mCurrentAnimation;
	};

}

#endif /* INCLUDE_ANIM_SKELETONANIMATIONPLAYER_H_ */
