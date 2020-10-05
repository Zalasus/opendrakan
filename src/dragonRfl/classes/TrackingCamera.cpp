/*
 * Camera.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/classes/TrackingCamera.h>

#include <dragonRfl/RflDragon.h>
#include <dragonRfl/LocalPlayer.h>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Client.h>

#include <odCore/input/InputManager.h>
#include <odCore/input/InputListener.h>

#include <odCore/rfl/Rfl.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Camera.h>

#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{

	TrackingCameraFields::TrackingCameraFields()
	: trackingMode(CameraTrackingMode::RUBBER_BAND)
	, rubberBandStrength(2)
	, spinSpeed(20)
	, crosshairDistance(8)
	{
	}

	void TrackingCameraFields::probeFields(odRfl::FieldProbe &probe)
	{
	    probe("Camera Options")
                (trackingMode, "Tracking Mode")
                (rubberBandStrength, "Rubber Band Strength")
                (spinSpeed, "Spin Speed")
                (crosshairDistance, "Cross-hair Distance (lu)");
	}


    TrackingCamera_Cl::TrackingCamera_Cl(od::LevelObject &objectToTrack)
    : mObjectToTrack(objectToTrack)
    {
    }

	void TrackingCamera_Cl::onLoaded()
	{
	    auto &obj = getLevelObject();

	    obj.setSpawnStrategy(od::SpawnStrategy::Always);

        mInputListener = getClient().getInputManager().createInputListener();
        mInputListener->setMouseMoveCallback([this](auto pos){ _mouseHandler(pos); });
	}

	void TrackingCamera_Cl::onSpawned()
	{
        auto &obj = getLevelObject();

        //obj.getLevel().activateLayerPVS(obj.getAssociatedLayer());

	    // set initial view matrix
	    _setObjectPositionAndViewMatrix(obj.getPosition(), obj.getRotation());

	    obj.setEnableUpdate(true);
	}

	void TrackingCamera_Cl::onDespawned()
	{
	}

	void TrackingCamera_Cl::onUpdate(float relTime)
	{
	    updateCamera();
	}

	void TrackingCamera_Cl::onLayerChanged(od::Layer *from, od::Layer *to)
	{
	    //getLevelObject().getLevel().activateLayerPVS(to);
	}

    void TrackingCamera_Cl::updateCamera()
    {
        std::shared_ptr<odPhysics::Handle> trackedPhysicsHandle;
        if(mObjectToTrack.getSpawnableClassInstance() != nullptr)
        {
            trackedPhysicsHandle = mObjectToTrack.getPhysicsHandle();
        }

        glm::vec3::value_type maxDistance = 3;
        glm::vec3::value_type bounceBackDistance = 0.1; // TODO: calculate this based of FOV or something

        glm::vec3 eye = mObjectToTrack.getPosition();
        glm::quat lookDirectionQuat(glm::vec3(mPitch, mYaw, 0));
        glm::vec3 lookDirection = lookDirectionQuat * glm::vec3(0, 0, 1);

        // perform raycast to find obstacle closest point with unobstructed view of player
        glm::vec3 from = mObjectToTrack.getPosition();
        glm::vec3 to = from + lookDirection * maxDistance;

        static const odPhysics::PhysicsTypeMasks::Mask mask = odPhysics::PhysicsTypeMasks::Layer | odPhysics::PhysicsTypeMasks::LevelObject;

        odPhysics::RayTestResult result;
        bool hit = getClient().getPhysicsSystem().rayTestClosest(from, to, mask, trackedPhysicsHandle, result);
        if(!hit)
        {
            eye = to;

        }else
        {
            // we hit something. bounce a bit off of surface (like a reflected beam of light) so we don't clip into the object

            glm::vec3 reflectedDirection = lookDirection - 2*glm::dot(lookDirection, result.hitNormal)*result.hitNormal;
            reflectedDirection = glm::normalize(reflectedDirection);

            eye = result.hitPoint + reflectedDirection * bounceBackDistance;
        }

        _setObjectPositionAndViewMatrix(eye, lookDirectionQuat);
    }

    glm::vec2 TrackingCamera_Cl::cursorPosToYawPitch(const glm::vec2 &cursorPos)
    {
        // the tranlation to pitch/yaw is easier in NDC. convert from GUI space
        glm::vec2 posNdc(2*cursorPos.x - 1, 1 - 2*cursorPos.y);
        float pitch =  glm::half_pi<float>()*posNdc.y;
        float yaw   = -glm::pi<float>()*posNdc.x;
        return {yaw, pitch};
    }

    void TrackingCamera_Cl::_setObjectPositionAndViewMatrix(const glm::vec3 &eyepoint, const glm::quat &lookDirection)
    {
        glm::vec3 front = lookDirection * glm::vec3(0, 0, -1); // rynn's model's look direction is negative z!
        glm::vec3 up = lookDirection * glm::vec3(0, 1, 0);

        getLevelObject().setPosition(eyepoint);
        getLevelObject().setRotation(lookDirection);

        getClient().getRenderer().getCamera()->lookAt(eyepoint, eyepoint + front, up);
    }

    void TrackingCamera_Cl::_mouseHandler(const glm::vec2 &pos)
    {
        glm::vec2 yawPitch = TrackingCamera_Cl::cursorPosToYawPitch(pos);
        mYaw = yawPitch.x;
        mPitch = yawPitch.y;

        updateCamera();
    }

}
