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
#include <odCore/rfl/Field.h>
#include <odCore/Camera.h>

namespace dragonRfl
{

    class DragonRfl;

	class TrackingCamera : public odRfl::RflClass, public od::Camera
	{
	public:

		TrackingCamera(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;

		virtual void onLoaded(od::LevelObject &obj) override;
		virtual void onSpawned(od::LevelObject &obj) override;
		virtual void onDespawned(od::LevelObject &obj) override;

		// implement od::Camera
		virtual osg::Vec3f getEyePoint() const override;
        virtual void setOsgCamera(osg::Camera *osgCam) override;
        virtual od::LevelObject  &getLevelObject() override;

        void updateCamera();


	private:

        void _setObjectPositionAndViewMatrix(const osg::Vec3f &eyepoint, const osg::Quat &lookDirection);

		odRfl::Enum		mTrackingMode; // 0 = Cockpit, 1 = Rubber Band, 2 = Chase Fixed
		odRfl::Float    mRubberBandStrength;
		odRfl::Float    mSpinSpeed;
		odRfl::Float	mCrosshairDistance;

		osg::ref_ptr<osg::Camera> mOsgCamera;
		osg::ref_ptr<osg::NodeCallback> mCamUpdateCallback;
		osg::ref_ptr<od::LevelObject> mCameraLevelObject;
	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x001b, "System", "Tracking Camera", dragonRfl::TrackingCamera);

#endif /* INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_ */
