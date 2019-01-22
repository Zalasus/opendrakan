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

    /**
     * Interface for objects that need to accumulate movement from an animation.
     */
    class MotionAccumulator
    {
    public:

        enum class Mode
        {
            Bone,
            Accumulate,
            Ignore
        };

        typedef std::array<Mode, 3> AxesModes;

        virtual ~MotionAccumulator() = default;

        virtual void moveRelative(const glm::vec3 &relTranslation, float relTime) = 0;

        virtual Mode getXAccumulationMode() = 0;
        virtual Mode getYAccumulationMode() = 0;
        virtual Mode getZAccumulationMode() = 0;
    };

}

#endif /* INCLUDE_ODCORE_ANIM_MOTIONACCUMULATOR_H_ */
