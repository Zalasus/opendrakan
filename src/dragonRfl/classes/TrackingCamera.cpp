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
#include <odCore/Engine.h>

#include <odCore/rfl/Rfl.h>

#include <odCore/render/Renderer.h>

#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{

	TrackingCamera::TrackingCamera(DragonRfl &rfl)
	: mTrackingMode(1)
	, mRubberBandStrength(2)
	, mSpinSpeed(20)
	, mCrosshairDistance(8)
	, mRfl(rfl)
	, mCameraObject(nullptr)
	{
	}

	void TrackingCamera::probeFields(odRfl::FieldProbe &probe)
	{
	    probe.beginCategory("Camera Options");
	    probe.registerField(mTrackingMode, "Tracking Mode");
	    probe.registerField(mRubberBandStrength, "Rubber Band Strength");
	    probe.registerField(mSpinSpeed, "Spin Speed");
	    probe.registerField(mCrosshairDistance, "Cross-hair Distance (lu)");
	}

	void TrackingCamera::onLoaded(od::LevelObject &obj)
	{
	    mCameraObject = &obj;

	    obj.setSpawnStrategy(od::SpawnStrategy::Always);

	    odRender::Renderer *renderer = mRfl.getEngine().getRenderer();
	    if(renderer != nullptr)
	    {
	        mRenderCamera = renderer->getCamera();
	    }
	}

	void TrackingCamera::onSpawned(od::LevelObject &obj)
	{
	    // set initial view matrix
	    _setObjectPositionAndViewMatrix(obj.getPosition(), obj.getRotation());

	    if(mRfl.getLocalPlayer() == nullptr)
	    {
	        // no player to track~ however, the camera object is tracked by the sky, so it still should be present
	        return;
	    }

	    obj.setEnableRflUpdateHook(true);
	}

	void TrackingCamera::onDespawned(od::LevelObject &obj)
	{
	}

	void TrackingCamera::onUpdate(od::LevelObject &obj, float relTime)
	{
	    updateCamera();
	}

    void TrackingCamera::updateCamera()
    {
        LocalPlayer *player = mRfl.getLocalPlayer();
        if(player == nullptr)
        {
            return;
        }

        odPhysics::Handle *playerHandle = player->getPhysicsHandle();

        glm::vec3::value_type maxDistance = 3;
        glm::vec3::value_type bounceBackDistance = 0.1; // TODO: calculate this based of FOV or something

        glm::vec3 eye = player->getPosition();
        glm::quat lookDirectionQuat(glm::vec3(player->getPitch(), player->getYaw(), 0));
        glm::vec3 lookDirection = lookDirectionQuat * glm::vec3(0, 0, 1);

        // perform raycast to find obstacle closest point with unobstructed view of player
        glm::vec3 from = player->getPosition();
        glm::vec3 to = from + lookDirection * maxDistance;

        static const odPhysics::PhysicsTypeMasks::Mask mask = odPhysics::PhysicsTypeMasks::Layer | odPhysics::PhysicsTypeMasks::LevelObject;

        odPhysics::RayTestResult result;
        bool hit = mRfl.getEngine().getPhysicsSystem().rayTestClosest(from, to, mask, playerHandle, result);
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

    void TrackingCamera::_setObjectPositionAndViewMatrix(const glm::vec3 &eyepoint, const glm::quat &lookDirection)
    {
        glm::vec3 front = lookDirection * glm::vec3(0, 0, -1); // rynn's model's look direction is negative z!
        glm::vec3 up = lookDirection * glm::vec3(0, 1, 0);

        mCameraObject->setPosition(eyepoint);
        mCameraObject->setRotation(lookDirection);

        if(mRenderCamera != nullptr)
        {
            mRenderCamera->lookAt(eyepoint, eyepoint + front, up);
        }
    }


	OD_REGISTER_RFLCLASS(DragonRfl, TrackingCamera);

}
