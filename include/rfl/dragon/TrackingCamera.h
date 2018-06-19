/*
 * Camera.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_
#define INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_

#include <osg/Camera>

#include "rfl/RflClass.h"
#include "rfl/RflField.h"
#include "Camera.h"

namespace odRfl
{

	class TrackingCamera : public RflClass, public od::Camera
	{
	public:

		TrackingCamera();

		virtual void probeFields(RflFieldProbe &probe) override;

		virtual void loaded(od::Engine &engine, od::LevelObject *obj) override;
		virtual void spawned(od::LevelObject &obj) override;
		virtual void despawned(od::LevelObject &obj) override;

		// implement od::Camera
		virtual osg::Vec3f getEyePoint() const override;
        virtual void setOsgCamera(osg::Camera *osgCam) override;
        virtual od::LevelObject  &getLevelObject() override;

        void update();


	private:

        void _setObjectPositionAndViewMatrix(const osg::Vec3f &eyepoint, const osg::Quat &lookDirection);

		RflEnum		mTrackingMode; // 0 = Cockpit, 1 = Rubber Band, 2 = Chase Fixed
		RflFloat    mRubberBandStrength;
		RflFloat    mSpinSpeed;
		RflFloat	mCrosshairDistance;

		od::Engine *mEngine;
		osg::ref_ptr<osg::Camera> mOsgCamera;
		osg::ref_ptr<osg::NodeCallback> mCamUpdateCallback;
		osg::ref_ptr<od::LevelObject> mCameraLevelObject;
	};

}

#endif /* INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_ */
