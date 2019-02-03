/*
 * Rig.h
 *
 *  Created on: Dec 1, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RIG_H_
#define INCLUDE_ODCORE_RENDER_RIG_H_

#include <glm/mat4x4.hpp>

namespace odRender
{

    /**
     * Interface for passing a flattened skeleton to the renderer.
     */
    class Rig
    {
    public:

        virtual ~Rig() = default;

        virtual void setBoneTransform(size_t boneIndex, glm::mat4 &transform) = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_RIG_H_ */
