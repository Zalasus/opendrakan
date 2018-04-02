/*
 * MotionAnimator.cpp
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#include "anim/MotionAnimator.h"

namespace od
{

	class MotionAnimatorUpdateCallback : public osg::NodeCallback
	{
	public:

		MotionAnimatorUpdateCallback(MotionAnimator &animator)
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

		MotionAnimator &mAnimator;
	};



	MotionAnimator::MotionAnimator(osg::PositionAttitudeTransform *node)
	: mNode(node)
	, mUpdateCallback(new MotionAnimatorUpdateCallback(*this))
	, mLastSimTime(-1)
	{
		mNode->addUpdateCallback(mUpdateCallback);
	}

	MotionAnimator::~MotionAnimator()
	{
		mNode->removeUpdateCallback(mUpdateCallback);
	}

	void MotionAnimator::update(double simTime)
	{
		if(mLastSimTime < 0)
		{
			mLastSimTime = simTime;
			return;
		}

		double relTime = simTime - mLastSimTime;
		osg::Vec3 pos = mNode->getPosition();

		pos += mVelocity * relTime;

		mNode->setPosition(pos);

		mLastSimTime = simTime;
	}


}
