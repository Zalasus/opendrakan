/*
 * Camera.h
 *
 *  Created on: 25 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_CAMERA_H_
#define INCLUDE_CAMERA_H_

#include <osg/Camera>

#include "LevelObject.h"

namespace od
{

	class Camera
	{
	public:

		Camera(osg::Camera *osgCam);
		Camera(const Camera &c) = delete;
		Camera(Camera &c) = delete;
		~Camera() = default;




    private:

		osg::ref_ptr<osg::Camera> mOsgCam;

	};

}

#endif /* INCLUDE_CAMERA_H_ */
