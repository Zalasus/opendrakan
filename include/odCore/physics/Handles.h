/*
 * Handles.h
 *
 *  Created on: 8 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_HANDLES_H_
#define INCLUDE_ODCORE_PHYSICS_HANDLES_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/RefCounted.h>

namespace odPhysics
{

    class Handle : public od::RefCounted
    {
    public:

        virtual ~Handle() = default;

    };


    class ObjectHandle : public Handle
    {
    public:

        /**
         * @brief Enables or disables collision detection. Detection is enabled by default.
         */
        virtual void setEnableCollision(bool b) = 0;

        virtual void setPosition(const glm::vec3 &p) = 0;
        virtual void setOrientation(const glm::quat &q) = 0;
        virtual void setScale(const glm::vec3 &s) = 0;

    };


    class LayerHandle : public Handle
    {
    public:


    };


    class LightHandle : public Handle
    {
    public:

        virtual void setRadius(float radius) = 0;
        virtual void setPosition(glm::vec3 &pos) = 0;

    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_HANDLES_H_ */
