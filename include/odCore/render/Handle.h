/*
 * Handle.h
 *
 *  Created on: Nov 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_HANDLE_H_
#define INCLUDE_ODCORE_RENDER_HANDLE_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace odRender
{

    class Geometry;

    class Handle
    {
    public:

        virtual ~Handle() = default;

        virtual void setPosition(const glm::vec3 &pos) = 0;
        virtual void setOrientation(const glm::quat &orientation) = 0;
        virtual void setScale(const glm::vec3 &scale) = 0;

        /**
         * Returns any geometry that might be present in this render node. Note that this may return nullptr
         * for some nodes like level objects, the latter not having actual geometry, instead referring to the model's
         * node internally.
         */
        virtual Geometry *getGeometry() = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_HANDLE_H_ */
