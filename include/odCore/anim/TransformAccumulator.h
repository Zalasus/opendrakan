/*
 * TransformAccumulator.h
 *
 *  Created on: 3 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_ANIM_TRANSFORMACCUMULATOR_H_
#define INCLUDE_ANIM_TRANSFORMACCUMULATOR_H_

#include <glm/vec3.hpp>

namespace odAnim
{

    /**
     * Interface for objects that need to accumulate movement from an animation.
     */
    class TransformAccumulator
    {
    public:

        virtual ~TransformAccumulator() = default;

        virtual void moveRelative(const glm::vec3 &v) = 0;

    };

}

#endif /* INCLUDE_ANIM_TRANSFORMACCUMULATOR_H_ */
