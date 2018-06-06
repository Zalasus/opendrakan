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

            mCam->update();
        }

    private:

        TrackingCamera *mCam;
    };


	TrackingCamera::TrackingCamera()
	: mTrackingMode(1)
	, mRubberBandStrength(2)
	, mSpinSpeed(20)
	, mCrosshairDistance(8)
	, mEngine(nullptr)
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

	void TrackingCamera::loaded(od::Engine &engine, od::LevelObject *obj)
	{
	    mEngine = &engine;
	    mCameraLevelObject = obj;
	    engine.setCamera(this);
	}

	void TrackingCamera::spawned(od::LevelObject &obj)
	{
	    if(mOsgCamera == nullptr)
	    {
	        Logger::error() << "Camera object spawned with no OSG camera assigned. Prepare for chaos";
	    }

	    // attach update callback to player so we always get updated after player
	    mCamUpdateCallback = new CamUpdateCallback(this);
	    obj.getLevel().getPlayer()->getLevelObject().addUpdateCallback(mCamUpdateCallback);
	}

	void TrackingCamera::despawned(od::LevelObject &obj)
	{
	    obj.getLevel().getPlayer()->getLevelObject().removeUpdateCallback(mCamUpdateCallback);
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

    void TrackingCamera::update()
    {
        od::Player *player = mCameraLevelObject->getLevel().getPlayer();

        osg::Vec3f eye = player->getPosition();
        osg::Quat lookDirection = osg::Quat(player->getPitch(), osg::Vec3f(0, 0, 1)) * osg::Quat(player->getYaw() + M_PI/2, osg::Vec3f(0, 1, 0));
        osg::Vec3f front = lookDirection * osg::Vec3f(1, 0, 0);
        osg::Vec3f up = lookDirection * osg::Vec3f(0, 1, 0);

        // perform raycast to find obstacle closest point with unobstructed view of player
        osg::Vec3f from = player->getPosition();
        osg::Vec3f to = from + lookDirection * osg::Vec3f(-3, 0, 0);
        od::RaycastResult result;
        bool hit = mEngine->getLevel().getPhysicsManager().raycastClosest(from, to, result, &player->getLevelObject());
        if(!hit)
        {
            eye = to;

        }else
        {
            eye = result.hitPoint;
        }


        mOsgCamera->setViewMatrixAsLookAt(eye, eye + front, up);
    }

	OD_REGISTER_RFL_CLASS(0x001b, "Tracking Camera", TrackingCamera);
}
