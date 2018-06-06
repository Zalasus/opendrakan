/*
 * Camera.h
 *
 *  Created on: 25 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_CAMERA_H_
#define INCLUDE_CAMERA_H_

#include <osg/Vec3f>
#include <osg/Camera>

namespace od
{

	/**
	 * Interface for exposing the RFL provided camera object to the engine
	 */
	class Camera
	{
	public:

	    virtual ~Camera() = default;

		virtual osg::Vec3f getEyePoint() const = 0;
		virtual void setOsgCamera(osg::Camera *osgCam) = 0;
	};

}

#endif /* INCLUDE_CAMERA_H_ */
