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
#include <osg/PositionAttitudeTransform>
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

		/**
		 * @param[in]   objectRoot         Parent node of the geometry. Rigging shader is attached to this.
		 * @param[in]   skeletonRoot       Parent group containing bone tree. The bone upload callback is attached to this.
		 * @param[in]   accumulatingXform  Transform accumulating relative movement of root node. May be nullptr.
		 */
		SkeletonAnimationPlayer(Engine &engine, osg::Node *objectRoot, osg::Group *skeletonRoot, osg::PositionAttitudeTransform *accumulatingXform); // we need an engine instance to load the rigging shader
		~SkeletonAnimationPlayer();

		inline osg::Uniform *getBoneMatrixArrayUniform() { return mBoneMatrixArray.get(); }
		inline Animation *getCurrentAnimation() { return mCurrentAnimation; }

		void setAnimation(osg::ref_ptr<Animation> anim, double startDelay = 0.0);
		void play(bool looping);
		void stop();
		bool isPlaying();

	private:

		Engine &mEngine;
		osg::ref_ptr<osg::Node> mObjectRoot;
		osg::ref_ptr<osg::Group> mSkeletonRoot;
		osg::ref_ptr<osg::PositionAttitudeTransform> mAccumulatingXform;
		osg::ref_ptr<osg::Uniform> mBoneMatrixArray;
		std::vector<osg::ref_ptr<Animator>> mAnimators;
		osg::ref_ptr<osg::Program> mRiggingProgram;
		osg::ref_ptr<osg::NodeCallback> mUploadCallback;

		osg::ref_ptr<Animation> mCurrentAnimation;
	};

}

#endif /* INCLUDE_ANIM_SKELETONANIMATIONPLAYER_H_ */
