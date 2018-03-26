/*
 * Animator.cpp
 *
 *  Created on: 24 Mar 2018
 *      Author: zal
 */

#include "anim/Animator.h"

#include "Logger.h"

namespace od
{

	class AnimatorUpdateCallback : public osg::NodeCallback
	{
	public:

		AnimatorUpdateCallback(Animator &animator)
		: mAnimator(animator)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			if(nv->getFrameStamp() != nullptr)
			{
				mAnimator.update(nv->getFrameStamp()->getSimulationTime());
			}

			traverse(node, nv);
		}

	private:

		Animator &mAnimator;
	};



	Animator::Animator(osg::MatrixTransform *node)
	: mNode(node)
	, mUpdateCallback(new AnimatorUpdateCallback(*this))
	, mLastKeyframeIndex(-1)
	, mPlayState(AnimationPlayState::STOPPED)
	{
		mNode->addUpdateCallback(mUpdateCallback);
	}

	Animator::~Animator()
	{
		mNode->removeUpdateCallback(mUpdateCallback);
	}

	void Animator::update(double simTime)
	{

	}

	void Animator::reserveKeyframes(size_t count)
	{
		if(count > 0)
		{
			mKeyframes.reserve(count);
		}
	}

	void Animator::addKeyframe(double time, const osg::Matrix &xform)
	{
		Keyframe kf;
		kf.time = time;
		kf.xform = xform;
		mKeyframes.push_back(kf);
	}

}
