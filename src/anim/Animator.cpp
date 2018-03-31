/*
 * Animator.cpp
 *
 *  Created on: 24 Mar 2018
 *      Author: zal
 */

#include "anim/Animator.h"

#include <cmath>
#include <osg/Matrix>

#include "Logger.h"
#include "Exception.h"

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
	, mOriginalXform(mNode->getMatrix())
	, mLastKeyframeIndex(-1)
	, mPlayState(AnimationPlayState::STOPPED)
	, mJustStarted(false)
	, mStartTime(0)
	, mDecompositionsDirty(true)
	{
		mNode->addUpdateCallback(mUpdateCallback);
	}

	Animator::~Animator()
	{
		mNode->removeUpdateCallback(mUpdateCallback);
	}

	void Animator::setPlayState(AnimationPlayState state)
	{
		mPlayState = state;

		switch(state)
		{
		case AnimationPlayState::PLAYING:
		case AnimationPlayState::PLAYING_LOOPED:
			mCurrentFrame = mAnimBegin;
			mJustStarted = true;
			break;

		default:
			break;
		}
	}

	void Animator::setKeyframes(std::vector<AnimationKeyframe>::iterator begin, std::vector<AnimationKeyframe>::iterator end)
	{
		size_t frameCount = end - begin;

		mAnimBegin = begin;
		mAnimLastFrame = begin + frameCount;
		mAnimEnd = end;
	}

	void Animator::update(double simTime)
	{
		if(mPlayState == AnimationPlayState::STOPPED)
		{
			return;

		}else if(mJustStarted)
		{
			mStartTime = simTime;
			mJustStarted = false;
		}

		// if needed, advance current frame to the next frame whose time lies just past the current relative time
		double relativeTime = simTime - mStartTime;
		while(mCurrentFrame < mAnimLastFrame)
		{
			if(mCurrentFrame->time <= relativeTime && (mCurrentFrame+1)->time > relativeTime)
			{
				break;
			}

			++mCurrentFrame;
			mDecompositionsDirty = true;
		}

		// did we advance past the last frame? if yes, stop animation or loop
		if(mCurrentFrame >= mAnimLastFrame)
		{
			mCurrentFrame = mAnimBegin;

			if(mPlayState == AnimationPlayState::PLAYING_LOOPED)
			{
				mJustStarted = true;

			}else
			{
				mPlayState == AnimationPlayState::STOPPED;
			}

			return;
		}

		// anim is still running. need to interpolate between mCurrentFrame and mCurrentFrame+1
		// although better interpolation methods exist, for now we just decompose the two xforms into translation, rotation and scale
		//  and interpolate these values linaraly
		auto nextFrame = mCurrentFrame+1;
		double delta = (nextFrame->time - mCurrentFrame->time)/relativeTime; // 0=exactly at current frame, 1=exactly at next frame
		if(mDecompositionsDirty)
		{
			osg::Quat  dummyOrient; // i don't think we need the scale orientation
			mCurrentFrame->xform.decompose(mLeftTrans, mLeftRot, mLeftScale, dummyOrient);
			nextFrame->xform.decompose(mRightTrans, mRightRot, mRightScale, dummyOrient);
			mDecompositionsDirty = false;
		}

		osg::Vec3f iTrans = mLeftTrans*delta + mRightTrans*(1-delta);
		osg::Vec3f iScale = mLeftScale*delta + mRightScale*(1-delta);
		osg::Quat iRot;
		iRot.slerp(delta, mLeftRot, mRightRot);

		osg::Matrixf iXform = osg::Matrixf::translate(iTrans);
		iXform.preMultRotate(iRot);
		iXform.preMultScale(iScale);

		mNode->setMatrix(iXform);
	}

}
