/*
 * Camera.h
 *
 *  Created on: 25 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_CAMERA_H_
#define INCLUDE_CAMERA_H_

#include <osg/Camera>
#include <osg/NodeCallback>

#include "LevelObject.h"

namespace od
{

	class Engine;

	/**
	 * Camera wrapper enabling player tracking, rubber band stuff etc.
	 */
	class Camera
	{
	public:

		Camera(Engine &engine, osg::Camera *osgCam);
		Camera(const Camera &c) = delete;
		Camera(Camera &c) = delete;
		~Camera();


		void update();


    private:

		Engine &mEngine;
		osg::ref_ptr<osg::Camera> mOsgCam;
		osg::ref_ptr<osg::NodeCallback> mUpdateCallback;

	};

}

#endif /* INCLUDE_CAMERA_H_ */
