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

        void setPosition(const glm::vec3 &pos);
        void setOrientation(const glm::quat &orientation);
        void setScale(const glm::vec3 &scale);

        /**
         * Returns any geometry that might be present in this render node. Note that this may return nullptr
         * for some nodes like level objects, the latter not having actual geometry, instead referring to the model's
         * node internally.
         */
        Geometry *getGeometry();


    };

}


#endif /* INCLUDE_ODCORE_RENDER_HANDLE_H_ */
