/*
 * Camera.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include "rfl/dragon/TrackingCamera.h"

#include <osg/NodeCallback>

#include "rfl/Rfl.h"
#include "Level.h"
#include "LevelObject.h"
#include "Engine.h"
#include "Player.h"

namespace odRfl
{


    class CamUpdateCallback : public osg::NodeCallback
    {
    public:

        CamUpdateCallback(TrackingCamera *cam)
        : mCam(cam)
        {
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            traverse(node, nv);

            mCam->updateCamera();
        }

    private:

        TrackingCamera *mCam;
    };


	TrackingCamera::TrackingCamera()
	: mTrackingMode(1)
	, mRubberBandStrength(2)
	, mSpinSpeed(20)
	, mCrosshairDistance(8)
	{
	}

	void TrackingCamera::probeFields(RflFieldProbe &probe)
	{
	    probe.beginCategory("Camera Options");
	    probe.registerField(mTrackingMode, "Tracking Mode");
	    probe.registerField(mRubberBandStrength, "Rubber Band Strength");
	    probe.registerField(mSpinSpeed, "Spin Speed");
	    probe.registerField(mCrosshairDistance, "Cross-hair Distance (lu)");
	}

	void TrackingCamera::onLoaded(od::LevelObject &obj)
	{
	    od::Engine &engine = obj.getLevel().getEngine();

	    if(engine.getCamera() != nullptr)
	    {
	        Logger::warn() << "Multiple camera objects found in level. Destroying duplicate";
	        obj.requestDestruction();
	        return;
	    }

	    mCameraLevelObject = &obj;
	    engine.setCamera(this);
	    obj.setSpawnStrategy(od::SpawnStrategy::Always);
	}

	void TrackingCamera::onSpawned(od::LevelObject &obj)
	{
	    od::Engine &engine = obj.getLevel().getEngine();

	    if(mOsgCamera == nullptr)
	    {
	        Logger::error() << "Camera object spawned with no OSG camera assigned. Prepare for chaos";
	        return;
	    }

	    // set initial view matrix
	    _setObjectPositionAndViewMatrix(obj.getPosition(), obj.getRotation());

	    if(engine.getPlayer() == nullptr)
	    {
	        // no player to track~ however, the camera object is tracked by the sky, so it still should be present
	        return;
	    }

	    // attach update callback to player so we always get updated after player
	    mCamUpdateCallback = new CamUpdateCallback(this);
	    engine.getPlayer()->getLevelObject().addUpdateCallback(mCamUpdateCallback);
	}

	void TrackingCamera::onDespawned(od::LevelObject &obj)
	{
	    od::Engine &engine = obj.getLevel().getEngine();

	    if(mCamUpdateCallback != nullptr && engine.getPlayer() != nullptr)
	    {
	        engine.getPlayer()->getLevelObject().removeUpdateCallback(mCamUpdateCallback);
	    }
	}

	osg::Vec3f TrackingCamera::getEyePoint() const
	{
	    return mCameraLevelObject->getPosition();
	}

    void TrackingCamera::setOsgCamera(osg::Camera *osgCam)
    {
        if(mOsgCamera != nullptr)
        {
            Logger::warn() << "Double assigned OSG camera to RFL TrackingCamera. Ignoring";
            return;
        }

        mOsgCamera = osgCam;
    }

    od::LevelObject &TrackingCamera::getLevelObject()
    {
        if(mCameraLevelObject == nullptr)
        {
            throw od::Exception("No level object assigned to RFL camera");
        }

        return *mCameraLevelObject;
    }

    void TrackingCamera::updateCamera()
    {
        od::Engine &engine = mCameraLevelObject->getLevel().getEngine();

        od::Player *player = engine.getPlayer();
        if(player == nullptr)
        {
            return;
        }

        osg::Vec3f eye = player->getPosition();
        osg::Quat lookDirection = osg::Quat(player->getPitch(), osg::Vec3f(0, 0, 1)) * osg::Quat(player->getYaw() + M_PI/2, osg::Vec3f(0, 1, 0));

        // perform raycast to find obstacle closest point with unobstructed view of player
        osg::Vec3f from = player->getPosition();
        osg::Vec3f to = from + lookDirection * osg::Vec3f(-3, 0, 0);
        od::RaycastResult result;
        bool hit = engine.getLevel().getPhysicsManager().raycastClosest(from, to, result, &player->getLevelObject());
        if(!hit)
        {
            eye = to;

        }else
        {
            eye = result.hitPoint;
        }

        _setObjectPositionAndViewMatrix(eye, lookDirection);
    }

    void TrackingCamera::_setObjectPositionAndViewMatrix(const osg::Vec3f &eyepoint, const osg::Quat &lookDirection)
    {
        osg::Vec3f front = lookDirection * osg::Vec3f(1, 0, 0);
        osg::Vec3f up = lookDirection * osg::Vec3f(0, 1, 0);

        mCameraLevelObject->setPosition(eyepoint);
        mCameraLevelObject->setRotation(lookDirection);

        mOsgCamera->setViewMatrixAsLookAt(eyepoint, eyepoint + front, up);
    }

	OD_REGISTER_RFL_CLASS(0x001b, "Tracking Camera", TrackingCamera);
}
