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
	, mPlayState(AnimationPlayState::STOPPED)
	, mKeyframeCount(0)
	, mJustStarted(false)
	, mStartTime(0)
	, mTimeScale(1)
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

	void Animator::setKeyframes(std::vector<AnimationKeyframe>::const_iterator begin, std::vector<AnimationKeyframe>::const_iterator end)
	{
		if(mPlayState != AnimationPlayState::PLAYING)
		{
			setPlayState(AnimationPlayState::STOPPED);
		}

		mKeyframeCount = end - begin;

		if(mKeyframeCount == 0)
		{
			Logger::warn() << "Tried to give Animator empty keyframe list. Ignoring call";

		}else if(mKeyframeCount == 1)
		{
			mAnimBegin = begin;

		}else
		{
			mAnimBegin = begin;
			mAnimLastFrame = begin + mKeyframeCount;
			mAnimEnd = end;
		}
	}

	void Animator::update(double simTime)
	{
		if(mPlayState == AnimationPlayState::STOPPED || mKeyframeCount == 0)
		{
			return;

		}else if(mKeyframeCount == 1)
		{
			// single frame animation i.e. static pose. just apply transform and don't bother with interpolations etc.
			mNode->setMatrix(mAnimBegin->xform * mOriginalXform);
			mPlayState = AnimationPlayState::STOPPED;
			mJustStarted = false;

		}else if(mJustStarted)
		{
			mStartTime = simTime;
            mLastITranslation = osg::Vec3(0,0,0);
			mJustStarted = false;
		}

		// if needed, advance current frame to the next frame whose time lies just past the current relative time
		double relativeTime = mTimeScale*(simTime - mStartTime);
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
				mPlayState = AnimationPlayState::STOPPED;
			}

			return;
		}

		// anim is still running. need to interpolate between mCurrentFrame and mCurrentFrame+1
		// although better interpolation methods exist, for now we just decompose the two xforms into translation, rotation and scale
		//  and interpolate these values linearly
		auto nextFrame = mCurrentFrame+1;
		double delta = (relativeTime - mCurrentFrame->time)/(nextFrame->time - mCurrentFrame->time); // 0=exactly at current frame, 1=exactly at next frame
		if(mDecompositionsDirty)
		{
			mCurrentFrame->xform.decompose(mLeftTrans, mLeftRot, mLeftScale, mLeftScaleOrient);
			nextFrame->xform.decompose(mRightTrans, mRightRot, mRightScale, mRightScaleOrient);
			mDecompositionsDirty = false;
		}

		osg::Vec3f iTrans = mLeftTrans*(1-delta) + mRightTrans*delta;
		osg::Vec3f iScale = mLeftScale*(1-delta) + mRightScale*delta;
		osg::Quat iRot;
		iRot.slerp(delta, mLeftRot, mRightRot);
		osg::Quat iOrient;
		iOrient.slerp(delta, mLeftScaleOrient, mRightScaleOrient);

		osg::Matrix iXform = mOriginalXform;
		iXform.preMultScale(iScale);
		iXform.preMultTranslate(iTrans);
		iXform.preMultRotate(iRot);

		// FIXME: this behaviour could be modeled better by making the skeleton root the model root and animating it relative to it's starting xform
		if(mAccumulatingXform != nullptr)
		{
			osg::Vec3 relTranslation = iTrans - mLastITranslation;
			relTranslation = relTranslation * osg::Matrix::rotate(-M_PI/2, osg::Vec3(0, 1, 0));
			mAccumulatingXform->setPosition(relTranslation + mAccumulatingXform->getPosition());
			mLastITranslation = iTrans;

		}else
		{
		    mNode->setMatrix(iXform);
		}
	}

}
