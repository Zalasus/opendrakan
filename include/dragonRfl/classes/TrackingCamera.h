/*
 * Camera.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_
#define INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dragonRfl
{

    class DragonRfl;

	class TrackingCamera : public odRfl::RflClass
	{
	public:

		TrackingCamera(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;

		virtual void onLoaded(od::LevelObject &obj) override;
		virtual void onSpawned(od::LevelObject &obj) override;
		virtual void onDespawned(od::LevelObject &obj) override;
		virtual void onUpdate(od::LevelObject &obj, double simTime, double relTime) override;

        void updateCamera();


	private:

        void _setObjectPositionAndViewMatrix(const glm::vec3 &eyepoint, const glm::quat &lookDirection);

		odRfl::Enum		mTrackingMode; // 0 = Cockpit, 1 = Rubber Band, 2 = Chase Fixed
		odRfl::Float    mRubberBandStrength;
		odRfl::Float    mSpinSpeed;
		odRfl::Float	mCrosshairDistance;

		DragonRfl &mRfl;
		od::LevelObject *mCameraObject;
	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x001b, "System", "Tracking Camera", dragonRfl::TrackingCamera);

#endif /* INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_ */
