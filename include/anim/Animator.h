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

#include "db/Skeleton.h"

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
		inline void setPlayState(AnimationPlayState state) { mPlayState = state; }

		void update(double simTime);

		void reserveKeyframes(size_t count);
		void addKeyframe(double time, const osg::Matrix &xform);


	private:

		struct Keyframe
		{
			double time;
			osg::Matrix xform;
		};

		osg::ref_ptr<osg::MatrixTransform> mNode;
		osg::ref_ptr<osg::NodeCallback> mUpdateCallback;
		std::vector<Keyframe> mKeyframes;
		int32_t mLastKeyframeIndex;
		AnimationPlayState mPlayState;
	};

}

#endif /* INCLUDE_ANIMATOR_H_ */
