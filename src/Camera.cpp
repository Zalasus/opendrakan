/*
 * Camera.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include "Camera.h"

#include "Engine.h"

namespace od
{

	class CamUpdateCallback : public osg::NodeCallback
	{
	public:

		CamUpdateCallback(Camera *cam)
		: mCam(cam)
		{
		}

		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
		{
			traverse(node, nv);

			mCam->update();
		}

	private:

		Camera *mCam;
	};



	Camera::Camera(Engine &engine, osg::Camera *osgCam)
	: mEngine(engine)
	, mOsgCam(osgCam)
	{
		mUpdateCallback = new CamUpdateCallback(this);
		mOsgCam->addUpdateCallback(mUpdateCallback);
	}

	Camera::~Camera()
    {
        mOsgCam->removeUpdateCallback(mUpdateCallback);
    }

	osg::Vec3 Camera::getEyePoint()
	{
		osg::Vec3 eye;
		osg::Vec3 center;
		osg::Vec3 up;

		mOsgCam->getViewMatrixAsLookAt(eye, center, up);

		return eye;
	}

	void Camera::update()
	{
		Player *player = mEngine.getPlayer();
		if(player == nullptr)
		{
			return;
		}

		osg::Quat lookDirection = osg::Quat(player->getPitch(), osg::Vec3f(0, 0, 1)) * osg::Quat(player->getYaw() + M_PI/2, osg::Vec3f(0, 1, 0));
		osg::Vec3f eye = player->getPosition();
		eye += lookDirection * osg::Vec3f(-2, 0, 0);
		osg::Vec3f front = lookDirection * osg::Vec3f(1, 0, 0);
		osg::Vec3f up = lookDirection * osg::Vec3f(0, 1, 0);

		mOsgCam->setViewMatrixAsLookAt(eye, eye + front, up);
	}




}
