/*
 * BoundingBox.cpp
 *
 *  Created on: 4 Nov 2018
 *      Author: zal
 */

#include <odCore/BoundingBox.h>

namespace od
{

    AxisAlignedBoundingBox::AxisAlignedBoundingBox(const glm::vec3 &min, const glm::vec3 &max)
    : mMin(min)
    , mMax(max)
    {
    }

    bool AxisAlignedBoundingBox::intersects(const AxisAlignedBoundingBox &aabb) const
    {
        return    aabb.mMin.x >= mMin.x && aabb.mMin.x <= mMax.x
               && aabb.mMin.y >= mMin.y && aabb.mMin.y <= mMax.y
               && aabb.mMin.z >= mMin.z && aabb.mMin.z <= mMax.z;
    }

    bool AxisAlignedBoundingBox::intersects(const AxisAlignedBoundingBox &aabb, glm::vec3::value_type epsilon) const
    {
        glm::vec3 eMin = mMin - epsilon;
        glm::vec3 eMax = mMax + epsilon;

        return    aabb.mMin.x >= eMin.x && aabb.mMin.x <= eMax.x
               && aabb.mMin.y >= eMin.y && aabb.mMin.y <= eMax.y
               && aabb.mMin.z >= eMin.z && aabb.mMin.z <= eMax.z;
    }

}

