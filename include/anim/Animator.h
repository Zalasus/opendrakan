/*
 * Animator.h
 *
 *  Created on: 24 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_ANIMATOR_H_
#define INCLUDE_ANIMATOR_H_

#include <vector>
#include <osg/MatrixTransform>

#include "db/Animation.h"

namespace od
{
	enum class AnimationPlayState
	{
		STOPPED,
		PLAYING,
		PLAYING_LOOPED
	};

	/**
	 * Class managing interpolated animation of a single osg::MatrixTransform.
	 */
	class Animator : public osg::Referenced
	{
	public:

		Animator(osg::MatrixTransform *node);
		Animator(const Animator &) = delete;
		Animator(Animator &) = delete;
		~Animator();

		inline AnimationPlayState getPlayState() const { return mPlayState; }
		inline osg::MatrixTransform *getNode() { return mNode; }

		void setPlayState(AnimationPlayState state);
		void setKeyframes(std::vector<AnimationKeyframe>::iterator begin, std::vector<AnimationKeyframe>::iterator end);

		void update(double simTime);



	protected:

		osg::ref_ptr<osg::MatrixTransform> mNode;
		osg::ref_ptr<osg::NodeCallback> mUpdateCallback;
		osg::Matrix mOriginalXform;
		AnimationPlayState mPlayState;
		size_t mKeyframeCount;
		bool mJustStarted;
		double mStartTime;
		std::vector<AnimationKeyframe>::iterator mAnimBegin;
		std::vector<AnimationKeyframe>::iterator mAnimLastFrame;
		std::vector<AnimationKeyframe>::iterator mAnimEnd;
		std::vector<AnimationKeyframe>::iterator mCurrentFrame;

		// matrix decompositions for interpolation
		bool mDecompositionsDirty;
		osg::Vec3f mLeftTrans;
		osg::Quat  mLeftRot;
		osg::Vec3f mLeftScale;
		osg::Quat  mLeftScaleOrient;
		osg::Vec3f mRightTrans;
		osg::Quat  mRightRot;
		osg::Vec3f mRightScale;
		osg::Quat  mRightScaleOrient;
	};

}

#endif /* INCLUDE_ANIMATOR_H_ */
