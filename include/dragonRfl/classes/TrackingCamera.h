/*
 * Camera.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_
#define INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>

#include <odCore/render/Camera.h>

namespace dragonRfl
{

	class TrackingCamera : public odRfl::LevelObjectClassBase
	{
	public:

		TrackingCamera();

		virtual void probeFields(odRfl::FieldProbe &probe) override;

		virtual void onLoaded() override;
		virtual void onSpawned() override;
		virtual void onDespawned() override;
		virtual void onUpdate(float relTime) override;

        void updateCamera();


	private:

        void _setObjectPositionAndViewMatrix(const glm::vec3 &eyepoint, const glm::quat &lookDirection);

		odRfl::Enum		mTrackingMode; // 0 = Cockpit, 1 = Rubber Band, 2 = Chase Fixed
		odRfl::Float    mRubberBandStrength;
		odRfl::Float    mSpinSpeed;
		odRfl::Float	mCrosshairDistance;

		od::RefPtr<odRender::Camera> mRenderCamera;
	};

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::TrackingCamera, 0x001b, "System", "Tracking Camera");

#endif /* INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_ */
