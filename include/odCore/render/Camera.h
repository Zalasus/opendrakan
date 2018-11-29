/*
 * Camera.h
 *
 *  Created on: Nov 29, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_CAMERA_H_
#define INCLUDE_ODCORE_RENDER_CAMERA_H_

#include <glm/vec3.hpp>

#include <odCore/RefCounted.h>

namespace odRender
{

    class Camera : public od::RefCounted
    {
    public:

        virtual ~Camera() = default;

        virtual glm::vec3 getEyePoint() = 0;
        virtual void lookAt(const glm::vec3 &eye, const glm::vec3 &up, const glm::vec3 &front) = 0;


    };

}


#endif /* INCLUDE_ODCORE_RENDER_CAMERA_H_ */
