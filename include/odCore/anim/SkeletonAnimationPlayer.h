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

#include <odCore/anim/AnimationPlayer.h>
#include <odCore/anim/Animator.h>
#include <odCore/db/Animation.h>

namespace od
{

	class TransformAccumulator;

	/**
	 * Extension of AnimationPlayer allowing to load animations from riot database assets and distributing their keyframes
	 * among the AnimationPlayer's Animator objects. This also manages loading the rigging shader and uploading
	 * the bone matrices to the GPU.
	 */
	class SkeletonAnimationPlayer : public AnimationPlayer
	{
	public:

		/**
		 * @param[in]   engine             Engine instance. Required for loading rigging shader.
		 * @param[in]   objectRoot         Parent node of the geometry. Rigging shader is attached to this.
		 * @param[in]   skeletonRoot       Parent group containing bone tree. The bone upload callback is attached to this.
		 * @param[in]   accumulator        TransformAccumulator to receive relative movement of root node. May be nullptr if none needed.
		 */
		SkeletonAnimationPlayer(osg::Node *objectRoot, osg::Group *skeletonRoot, TransformAccumulator *accumulator);
		~SkeletonAnimationPlayer();

		inline osg::Uniform *getBoneMatrixArrayUniform() { return mBoneMatrixArray.get(); }
		inline Animation *getCurrentAnimation() { return mCurrentAnimation; }

		void setAnimation(osg::ref_ptr<Animation> anim, double startDelay = 0.0);
		void play(bool looping);
		void stop();
		bool isPlaying();

	private:

		osg::ref_ptr<osg::Node> mObjectRoot;
		osg::ref_ptr<osg::Group> mSkeletonRoot;
		TransformAccumulator *mAccumulator;
		osg::ref_ptr<osg::Uniform> mBoneMatrixArray;
		std::vector<osg::ref_ptr<Animator>> mAnimators;
		osg::ref_ptr<osg::NodeCallback> mUploadCallback;

		osg::ref_ptr<Animation> mCurrentAnimation;
	};

}

#endif /* INCLUDE_ANIM_SKELETONANIMATIONPLAYER_H_ */
