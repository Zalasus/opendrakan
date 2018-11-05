/*
 * Camera.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/classes/TrackingCamera.h>

#include <dragonRfl/RflDragon.h>
#include <dragonRfl/LocalPlayer.h>

#include <odCore/rfl/Rfl.h>
#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Engine.h>

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

	void TrackingCamera::onUpdate(od::LevelObject &obj, double simTime, double relTime)
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

        glm::vec3 eye = player->getPosition();
        glm::quat lookDirection(glm::vec3(player->getPitch(), player->getYaw() + M_PI/2, 0));

        // perform raycast to find obstacle closest point with unobstructed view of player
        glm::vec3 from = player->getPosition();
        glm::vec3 to = from + lookDirection * glm::vec3(-3, 0, 0);
        odPhysics::RaycastResult result;
        bool hit = player->getLevelObject().getLevel().getPhysicsManager().raycastClosest(from, to, result, &player->getLevelObject());
        if(!hit)
        {
            eye = to;

        }else
        {
            eye = result.hitPoint;
        }

        _setObjectPositionAndViewMatrix(eye, lookDirection);
    }

    void TrackingCamera::_setObjectPositionAndViewMatrix(const glm::vec3 &eyepoint, const glm::quat &lookDirection)
    {
        glm::vec3 front = lookDirection * glm::vec3(1, 0, 0);
        glm::vec3 up = lookDirection * glm::vec3(0, 1, 0);

        mCameraObject->setPosition(eyepoint);
        mCameraObject->setRotation(lookDirection);
    }


	OD_REGISTER_RFLCLASS(DragonRfl, TrackingCamera);

}
