/*
 * Animator.cpp
 *
 *  Created on: 24 Mar 2018
 *      Author: zal
 */

#include "anim/Animator.h"

#include <cmath>
#include <osg/Matrix>

#include "anim/TransformAccumulator.h"
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
	, mKeyframeCount(0)
	, mPlaying(false)
	, mLooping(false)
	, mJustStarted(false)
	, mStartDelay(0.0)
	, mStartTime(0.0)
	, mTimeScale(1.0)
	, mLeftTime(0.0)
	, mRightTime(0.0)
	, mLastInterpolatedScale(1,1,1)
	, mAccumulator(nullptr)
	, mAccumulationFactors(1,1,1)
	{
		mNode->addUpdateCallback(mUpdateCallback);
	}

	Animator::~Animator()
	{
		mNode->removeUpdateCallback(mUpdateCallback);
	}

	void Animator::setKeyframes(KfIterator begin, KfIterator end, double startDelay)
	{
	    size_t kfCount = end - begin;
	    if(kfCount <= 0)
	    {
	        Logger::warn() << "Tried to apply animation with zero or less frames to Animator. Ignoring call";
	        return;
	    }

	    mKeyframeCount = kfCount;
	    mJustStarted = true;

	    mAnimBegin = begin;
	    mCurrentFrame = mAnimBegin;
	    mAnimEnd = end;
	    mAnimLastFrame = begin + (kfCount - 1);

		mLeftTime = -startDelay;
		mRightTime = mAnimBegin->time;
		mStartDelay = startDelay;

		if(mAccumulator != nullptr)
		{
			mLastInterpolatedTranslation = osg::Vec3(0,0,0);
			mLastInterpolatedRotation = osg::Quat(0, osg::Vec3(0,1,0));
			mLastInterpolatedScale = osg::Vec3(1,1,1);
		}

        // use last interpolated transform as starting point so we can lerp between animations
        mLeftTranslation = mLastInterpolatedTranslation;
        mLeftRotation    = mLastInterpolatedRotation;
        mLeftScale       = mLastInterpolatedScale;

        osg::Quat dummyOrientation; // scale orientation can be safely ignored here
        mAnimBegin->xform.decompose(mRightTranslation, mRightRotation, mRightScale, dummyOrientation);
	}

	void Animator::play(bool looping)
	{
	    mPlaying = true;
	    mLooping = looping;

	    mJustStarted = true;
	}

    void Animator::stop()
    {
        mPlaying = false;
    }

	void Animator::update(double simTime)
	{
	    if(!mPlaying || mKeyframeCount == 0)
		{
			return;
		}

	    if(mJustStarted)
	    {
	        mStartTime = simTime;
	        mJustStarted = false;
	    }

		double relativeTime = simTime - mStartTime - mStartDelay;
		if(relativeTime >= mRightTime)
		{
		    if(mKeyframeCount == 1)
		    {
		        // FIXME: this ignores accumulating nodes when they only have one kf
		        mNode->setMatrix(mCurrentFrame->xform * mOriginalXform);
		        mPlaying = false;
		        return;
		    }

		    // advance until we are at valid position again
            while(mCurrentFrame < mAnimLastFrame)
            {
                if(mCurrentFrame->time <= relativeTime && (mCurrentFrame+1)->time > relativeTime)
                {
                    break;
                }

                ++mCurrentFrame;
            }

            // did we advance past the last frame? if yes, stop animation or loop
            if(mCurrentFrame >= mAnimLastFrame)
            {
                mCurrentFrame = mAnimBegin;

                if(mLooping)
                {
                    // when looping, it is important that we update relativeTime to incorporate any time we might have moved past the last frame
                    relativeTime = relativeTime - mAnimLastFrame->time;
                    mStartDelay = 0.0;
                    mStartTime = simTime - relativeTime;
                    mLastInterpolatedTranslation = osg::Vec3(0,0,0);
                    mLastInterpolatedRotation = osg::Quat(0, osg::Vec3(0,1,0));
                    mLastInterpolatedScale = osg::Vec3(1,1,1);

                }else
                {
                    mPlaying = false;
                }
            }

            mLeftTime = mCurrentFrame->time;
            mRightTime = (mCurrentFrame+1)->time;

            osg::Quat dummyOrientation; // scale orientation can be safely ignored here
            mCurrentFrame->xform.decompose(mLeftTranslation, mLeftRotation, mLeftScale, dummyOrientation);
            (mCurrentFrame+1)->xform.decompose(mRightTranslation, mRightRotation, mRightScale, dummyOrientation);
		}

		// anim is still running. need to interpolate between mCurrentFrame and mCurrentFrame+1
		// although better interpolation methods exist, for now we just decompose the two xforms into translation, rotation and scale
		//  and interpolate these values linearly
		double delta = (relativeTime - mLeftTime)/(mRightTime - mLeftTime); // 0=exactly at current frame, 1=exactly at next frame

		osg::Vec3f iTrans = mLeftTranslation*(1-delta) + mRightTranslation*delta;
		osg::Vec3f iScale = mLeftScale*(1-delta) + mRightScale*delta;
		osg::Quat iRot;
		iRot.slerp(delta, mLeftRotation, mRightRotation);



		if(mAccumulator != nullptr)
		{
			osg::Vec3 relTranslation = iTrans - mLastInterpolatedTranslation;
			relTranslation = osg::componentMultiply(relTranslation, mAccumulationFactors);
			mAccumulator->moveRelative(relTranslation);

			osg::Matrix iXform = mOriginalXform;
			iXform.preMultScale(iScale);
			iXform.preMultRotate(iRot);
		    mNode->setMatrix(iXform);

		}else
		{
			osg::Matrix iXform = mOriginalXform;
			iXform.preMultScale(iScale);
			iXform.preMultTranslate(iTrans);
			iXform.preMultRotate(iRot);
		    mNode->setMatrix(iXform);
		}

		mLastInterpolatedTranslation = iTrans;
		mLastInterpolatedRotation    = iRot;
		mLastInterpolatedScale       = iScale;
	}
}
