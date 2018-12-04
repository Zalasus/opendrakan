/*
 * Camera.h
 *
 *  Created on: Nov 29, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_CAMERA_H_
#define INCLUDE_ODOSG_CAMERA_H_

#include <osg/Camera>

#include <odCore/render/Camera.h>

namespace odOsg
{

    class Camera : public odRender::Camera
    {
    public:

        Camera(osg::Camera *osgCam);

        inline void setIgnoreViewChanges(bool b) { mIgnoreViewChanges = b; }

        virtual glm::vec3 getEyePoint() override;
        virtual void lookAt(const glm::vec3 &eye, const glm::vec3 &up, const glm::vec3 &front) override;


    private:

        osg::ref_ptr<osg::Camera> mOsgCamera;

        bool mIgnoreViewChanges;
    };

}


#endif /* INCLUDE_ODOSG_CAMERA_H_ */
