/*
 * BoundingBox.h
 *
 *  Created on: 4 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_BOUNDINGBOX_H_
#define INCLUDE_BOUNDINGBOX_H_

#include <glm/vec3.hpp>

namespace od
{

    class AxisAlignedBoundingBox
    {
    public:

        AxisAlignedBoundingBox(const glm::vec3 &min, const glm::vec3 &max);

    };

    class OrientedBoundingBox;

}

#endif /* INCLUDE_BOUNDINGBOX_H_ */
