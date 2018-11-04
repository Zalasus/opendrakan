/*
 * BoundingBox.h
 *
 *  Created on: 4 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_BOUNDINGBOX_H_
#define INCLUDE_BOUNDINGBOX_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace od
{

    class AxisAlignedBoundingBox
    {
    public:

        AxisAlignedBoundingBox(const glm::vec3 &min, const glm::vec3 &max);

        bool intersects(const AxisAlignedBoundingBox &aabb) const;
        bool intersects(const AxisAlignedBoundingBox &aabb, glm::vec3::value_type epsilon) const;


    private:

        glm::vec3 mMin;
        glm::vec3 mMax;
    };

    class OrientedBoundingBox
    {

    };

}

#endif /* INCLUDE_BOUNDINGBOX_H_ */
