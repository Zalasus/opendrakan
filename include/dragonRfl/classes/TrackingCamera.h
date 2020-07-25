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

    enum class CameraTrackingMode
    {
        COCKPIT = 0,
        RUBBER_BAND = 1,
        CHASE_FIXED = 2
    };


    struct TrackingCameraFields final : public odRfl::FieldBundle
    {
        TrackingCameraFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::EnumImpl<CameraTrackingMode> trackingMode;
		odRfl::Float                        rubberBandStrength;
		odRfl::Float                        spinSpeed;
		odRfl::Float	                    crosshairDistance;
    };


	class TrackingCamera_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<TrackingCamera_Cl>
	{
	public:

        TrackingCamera_Cl(od::LevelObject &objectToTrack);

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

		virtual void onLoaded() override;
		virtual void onSpawned() override;
		virtual void onDespawned() override;
		virtual void onUpdate(float relTime) override;
		virtual void onLayerChanged(od::Layer *from, od::Layer *to) override;

        void updateCamera();


	private:

        void _setObjectPositionAndViewMatrix(const glm::vec3 &eyepoint, const glm::quat &lookDirection);

		TrackingCameraFields mFields;

        od::LevelObject &mObjectToTrack;

		od::RefPtr<odRender::Camera> mRenderCamera;
	};

    class TrackingCameraFactory final : public odRfl::ClassFactory
    {
    public:

        virtual std::unique_ptr<odRfl::FieldBundle> makeFieldBundle()
        {
            return std::make_unique<TrackingCameraFields>():
        }

        virtual std::unique_ptr<ClassBase> makeInstance(od::Engine &engine)
        {
            // all cameras are instantiated as dummies initially. the RFL can decide to replace
            //  or clone them for each player (if in multiplayer)
            return std::make_unique<odRfl::DummyClass>();
        }

    };

    OD_DEFINE_CLASS(TrackingCamera, 0x001b, "System", "Tracking Camera", TrackingCameraFactory);

}

#endif /* INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_ */
