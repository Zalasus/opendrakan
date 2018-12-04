/*
 * Camera.cpp
 *
 *  Created on: Nov 29, 2018
 *      Author: zal
 */

#include <odOsg/Camera.h>

#include <odCore/Exception.h>

#include <odOsg/GlmAdapter.h>


namespace odOsg
{

    Camera::Camera(osg::Camera *osgCam)
    : mOsgCamera(osgCam)
    , mIgnoreViewChanges(false)
    {
        if(osgCam == nullptr)
        {
            throw od::InvalidArgumentException("Passed nullptr as camera");
        }
    }

    glm::vec3 Camera::getEyePoint()
    {
        osg::Vec3 eye;
        osg::Vec3 up;
        osg::Vec3 front;
        osg::Matrix viewMatrix = mOsgCamera->getViewMatrix();
        viewMatrix.getLookAt(eye, front, up, 1);

        return GlmAdapter::toGlm(eye);
    }

    void Camera::lookAt(const glm::vec3 &eye, const glm::vec3 &up, const glm::vec3 &front)
    {
        if(mIgnoreViewChanges)
        {
            return;
        }

        osg::Vec3 osgEye = GlmAdapter::toOsg(eye);
        osg::Vec3 osgUp = GlmAdapter::toOsg(up);
        osg::Vec3 osgFront = GlmAdapter::toOsg(front);

        mOsgCamera->setViewMatrixAsLookAt(osgEye, osgFront, osgUp);
    }


}



