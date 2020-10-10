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
#include <odCore/rfl/DummyClass.h>

#include <odCore/input/InputListener.h>

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
        virtual void onStart() override;
        virtual void onStop() override;
		virtual void onUpdate(float relTime) override;
		virtual void onLayerChanged(od::Layer *from, od::Layer *to) override;
        virtual void onTransformChanged() override;

        /// @brief Turns a position on the screen into a vector of yaw/pitch for the player.
        static glm::vec2 cursorPosToYawPitch(const glm::vec2 &p);

	private:

        void _mouseHandler(const glm::vec2 &pos);
        void _updateCameraTracking();
        void _updateCameraViewMatrix();

		TrackingCameraFields mFields;

        od::LevelObject &mObjectToTrack;

        std::shared_ptr<odInput::InputListener> mInputListener;

        float mYaw;
        float mPitch;
	};

    class TrackingCameraFactory final : public odRfl::ClassFactory
    {
    public:

        virtual std::unique_ptr<odRfl::FieldBundle> makeFieldBundle()
        {
            return std::make_unique<TrackingCameraFields>();
        }

        virtual std::unique_ptr<odRfl::ClassBase> makeInstance(od::Engine &engine)
        {
            // all cameras are instantiated as dummies initially. the RFL can decide to replace
            //  or clone them for each player (if in multiplayer)
            return std::make_unique<odRfl::DummyClass>();
        }

    };

    OD_DEFINE_CLASS(TrackingCamera, 0x001b, "System", "Tracking Camera", TrackingCameraFactory);

}

#endif /* INCLUDE_RFL_DRAGON_TRACKINGCAMERA_H_ */
