/*
 * Animator.h
 *
 *  Created on: 24 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_ANIMATOR_H_
#define INCLUDE_ANIMATOR_H_

#include <vector>
#include <deque>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include "db/Animation.h"

namespace od
{
	/**
	 * Class managing interpolated animation of a single osg::MatrixTransform.
	 */
	class Animator : public osg::Referenced
	{
	public:

	    typedef std::vector<AnimationKeyframe>::const_iterator KfIterator;

		Animator(osg::MatrixTransform *node);
		Animator(const Animator &) = delete;
		Animator(Animator &) = delete;
		~Animator();

		inline osg::MatrixTransform *getNode() { return mNode; }
		inline void setAccumulatingXform(osg::PositionAttitudeTransform *xform) { mAccumulatingXform = xform; }
		inline bool isPlaying() const { return mPlaying; }

		void setKeyframes(KfIterator begin, KfIterator end, double startDelay = 0.0f);
		void play(bool looping);
		void stop();

		void update(double simTime);


	protected:

		osg::ref_ptr<osg::MatrixTransform> mNode;
		osg::ref_ptr<osg::NodeCallback> mUpdateCallback;
		osg::Matrix mOriginalXform;
		size_t mKeyframeCount;
		bool mPlaying;
		bool mLooping;
		bool mJustStarted;
		double mStartDelay;
		double mStartTime;
		double mTimeScale;
		KfIterator mAnimBegin;
		KfIterator mAnimLastFrame;
		KfIterator mAnimEnd;
		KfIterator mCurrentFrame;

		double mLeftTime;
		double mRightTime;

		// matrix decompositions for interpolation
		osg::Vec3f mLeftTranslation;
		osg::Quat  mLeftRotation;
		osg::Vec3f mLeftScale;
		osg::Vec3f mRightTranslation;
		osg::Quat  mRightRotation;
		osg::Vec3f mRightScale;
		osg::Vec3f mLastInterpolatedTranslation;
		osg::Quat  mLastInterpolatedRotation;
		osg::Vec3f mLastInterpolatedScale;

		osg::ref_ptr<osg::PositionAttitudeTransform> mAccumulatingXform;
	};

}

#endif /* INCLUDE_ANIMATOR_H_ */
