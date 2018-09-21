/*
 * Camera.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_
#define INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_

#include <osg/Camera>
#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>
#include <odCore/Camera.h>

namespace od
{

	class TrackingCamera : public RflClass, public Camera
	{
	public:

		TrackingCamera();

		virtual void probeFields(FieldProbe &probe) override;

		virtual void onLoaded(LevelObject &obj) override;
		virtual void onSpawned(LevelObject &obj) override;
		virtual void onDespawned(LevelObject &obj) override;

		// implement od::Camera
		virtual osg::Vec3f getEyePoint() const override;
        virtual void setOsgCamera(osg::Camera *osgCam) override;
        virtual LevelObject  &getLevelObject() override;

        void updateCamera();


	private:

        void _setObjectPositionAndViewMatrix(const osg::Vec3f &eyepoint, const osg::Quat &lookDirection);

		RflEnum		mTrackingMode; // 0 = Cockpit, 1 = Rubber Band, 2 = Chase Fixed
		RflFloat    mRubberBandStrength;
		RflFloat    mSpinSpeed;
		RflFloat	mCrosshairDistance;

		osg::ref_ptr<osg::Camera> mOsgCamera;
		osg::ref_ptr<osg::NodeCallback> mCamUpdateCallback;
		osg::ref_ptr<LevelObject> mCameraLevelObject;
	};


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x001b, "System", "Tracking Camera", TrackingCamera);

}

#endif /* INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_ */
