/*
 * BoundingBox.cpp
 *
 *  Created on: 4 Nov 2018
 *      Author: zal
 */

#include <odCore/BoundingBox.h>

#include <limits>

#include <glm/mat3x3.hpp>
#include <glm/matrix.hpp>

#include <odCore/DataStream.h>

namespace od
{

    AxisAlignedBoundingBox::AxisAlignedBoundingBox()
    : mMin(std::numeric_limits<glm::vec3::value_type>::max())
    , mMax(-mMin)
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
        return !(mMax.x < aabb.mMin.x || mMin.x > aabb.mMax.x ||
                 mMax.y < aabb.mMin.y || mMin.y > aabb.mMax.y ||
                 mMax.z < aabb.mMin.z || mMin.z > aabb.mMax.z);
    }

    bool AxisAlignedBoundingBox::intersects(const AxisAlignedBoundingBox &aabb, glm::vec3::value_type epsilon) const
    {
        glm::vec3 eMin = mMin - epsilon;
        glm::vec3 eMax = mMax + epsilon;

        return !(eMax.x < aabb.mMin.x || eMin.x > aabb.mMax.x ||
                 eMax.y < aabb.mMin.y || eMin.y > aabb.mMax.y ||
                 eMax.z < aabb.mMin.z || eMin.z > aabb.mMax.z);
    }

    bool AxisAlignedBoundingBox::contains(const glm::vec3 &v) const
    {
        return    v.x >= mMin.x && v.x <= mMax.x
               && v.y >= mMin.x && v.y <= mMax.y
               && v.z >= mMin.x && v.z <= mMax.z;
    }

    void AxisAlignedBoundingBox::expandBy(const glm::vec3 &v)
    {
        if(contains(v))
        {
            return;
        }

        mMin = glm::min(mMin, v);
        mMax = glm::max(mMax, v);
    }


    OrientedBoundingBox::OrientedBoundingBox()
    : mExtends(0, 0, 0)
    , mCenter(0, 0, 0)
    , mOrientation(1, 0, 0, 0)
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
        // an OBB is defined by it's minimum corner and a scale*rotation matrix.
        //  we need to decompose that matrix to get the center+scale+orientation definition we use

        glm::vec3 cornerOffset;
        glm::mat3 xform;

        (*this) >> cornerOffset
                >> xform;

        // simple matrix decomposition for matrix with only scale and rotation:
        //  since a rotation matrix is orthonormal, and xform is calculated as scale*rot, we can find the scaling factors
        //  in the absolutes of the row vectors. thus, we first extract the scale factors, then factor in the inverse
        //  scaling matrix to get the rotation matrix (same as normalizing each row)

        xform = glm::transpose(xform); // transpose so we turn scale*rot into rot*scale, which means we can use column vectors

        glm::vec3 scale;
        scale.x = glm::length(xform[0]);
        scale.y = glm::length(xform[1]);
        scale.z = glm::length(xform[2]);

        glm::mat3 inverseScaleXform(1.0);
        inverseScaleXform[0][0] = 1.0/scale.x;
        inverseScaleXform[1][1] = 1.0/scale.y;
        inverseScaleXform[2][2] = 1.0/scale.z;

        glm::mat3 rotationXform(xform * inverseScaleXform);
        glm::quat orientation(rotationXform);

        glm::vec3 center = cornerOffset + orientation*(scale*0.5f);

        obb = OrientedBoundingBox(scale, center, orientation);

        return *this;
    }


}

