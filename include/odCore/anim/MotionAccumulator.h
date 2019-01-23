/*
 * MotionAccumulator.h
 *
 *  Created on: 22 Jan 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_ANIM_MOTIONACCUMULATOR_H_
#define INCLUDE_ODCORE_ANIM_MOTIONACCUMULATOR_H_

#include <array>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace odAnim
{

    enum class AccumulationMode
    {
        Bone,
        Accumulate,
        Ignore
    };

    typedef std::array<AccumulationMode, 3> AxesModes;

    /**
     * Interface for objects that need to accumulate movement from an animation.
     */
    class MotionAccumulator
    {
    public:

        virtual ~MotionAccumulator() = default;

        virtual void moveRelative(const glm::vec3 &relTranslation, float relTime) = 0;

    };

}

#endif /* INCLUDE_ODCORE_ANIM_MOTIONACCUMULATOR_H_ */
