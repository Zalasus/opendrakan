/*
 * BoundingBox.cpp
 *
 *  Created on: 4 Nov 2018
 *      Author: zal
 */

#include <odCore/BoundingBox.h>

#include <glm/mat3x3.hpp>

#include <odCore/DataStream.h>

namespace od
{

    AxisAlignedBoundingBox::AxisAlignedBoundingBox()
    : mMin(0, 0, 0)
    , mMax(0, 0, 0)
    {
    }

    AxisAlignedBoundingBox::AxisAlignedBoundingBox(const glm::vec3 &min, const glm::vec3 &max)
    : mMin(min)
    , mMax(max)
    {
    }

    AxisAlignedBoundingBox::AxisAlignedBoundingBox(const AxisAlignedBoundingBox &aabb)
    : mMin(aabb.mMin)
    , mMax(aabb.mMax)
    {
    }

    AxisAlignedBoundingBox &AxisAlignedBoundingBox::operator=(const AxisAlignedBoundingBox &aabb)
    {
        mMin = aabb.mMin;
        mMax = aabb.mMax;

        return *this;
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



    OrientedBoundingBox::OrientedBoundingBox()
    : mExtends(0, 0, 0)
    , mCenter(0, 0, 0)
    , mOrientation(0, 0, 0, 1)
    {
    }

    OrientedBoundingBox::OrientedBoundingBox(const glm::vec3 &extends, const glm::vec3 &center, const glm::quat &orientation)
    : mExtends(extends)
    , mCenter(center)
    , mOrientation(orientation)
    {
    }

    OrientedBoundingBox::OrientedBoundingBox(const OrientedBoundingBox &obb)
    : mExtends(obb.mExtends)
    , mCenter(obb.mCenter)
    , mOrientation(obb.mOrientation)
    {
    }

    OrientedBoundingBox &OrientedBoundingBox::operator=(const OrientedBoundingBox &obb)
    {
        mExtends = obb.mExtends;
        mCenter = obb.mCenter;
        mOrientation = obb.mOrientation;

        return *this;
    }

    template <>
    DataReader &DataReader::operator >> <OrientedBoundingBox>(OrientedBoundingBox &obb)
    {
        glm::vec3 center;
        glm::mat3 xform;

        (*this) >> center
                >> xform;

        // FIXME: fuck it. just needs to compile for now. implement this later!!!!

        return *this;
    }


}

