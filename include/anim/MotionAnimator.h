/*
 * MotionAnimator.h
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_ANIM_MOTIONANIMATOR_H_
#define INCLUDE_ANIM_MOTIONANIMATOR_H_

#include <osg/PositionAttitudeTransform>

namespace od
{

	/**
	 * Class for animating a single PositionAttitudeTransform with velocity and acceleration vectors.
	 *
	 * @deprecated This is completely unused and will probably stay that way. Motion is either animated by
	 * keyframes or via the physics system, so there's really no apparent reason for this class to exist.
	 * Even particles can be implemented better by using osg's particle system.
	 * This class will probably get removed soon.
	 */
	class MotionAnimator : public osg::Referenced
	{
	public:

		MotionAnimator(osg::PositionAttitudeTransform *node);
		MotionAnimator(const MotionAnimator &) = delete;
		MotionAnimator(MotionAnimator &) = delete;
		~MotionAnimator();

		/// sets velocity in units/s
		inline void setVelocity(osg::Vec3 v) { mVelocity = v; }
		inline osg::Vec3f getVelocity() const { return mVelocity; }

		/// sets angular velocity in rad/s
		inline void setRotVelocity(osg::Vec3 v) { mRotVelocity = v; }
		inline osg::Vec3f getRotVelocity() const { return mRotVelocity; }

		void update(double simTime);


	private:

		osg::ref_ptr<osg::PositionAttitudeTransform> mNode;
		osg::ref_ptr<osg::NodeCallback> mUpdateCallback;
		osg::Vec3 mVelocity;
		osg::Vec3 mRotVelocity;
		double mLastSimTime;
	};

}

#endif /* INCLUDE_ANIM_MOTIONANIMATOR_H_ */
