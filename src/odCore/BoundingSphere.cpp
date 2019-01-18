/*
 * BoundingSphere.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: zal
 */

#include <odCore/BoundingSphere.h>

#include <glm/gtx/norm.hpp>

#include <odCore/DataStream.h>

namespace od
{

    BoundingSphere::BoundingSphere()
    : mCenter(0, 0, 0)
    , mRadius(0)
    {
    }

    BoundingSphere::BoundingSphere(const BoundingSphere &bs)
    : mCenter(bs.mCenter)
    , mRadius(bs.mRadius)
    {
    }

    BoundingSphere::BoundingSphere(const glm::vec3 &center, glm::vec3::value_type radius)
    : mCenter(center)
    , mRadius(radius)
    {
    }

    BoundingSphere &BoundingSphere::operator=(const BoundingSphere &bs)
    {
        mCenter = bs.mCenter;
        mRadius = bs.mRadius;

        return *this;
    }

    bool BoundingSphere::intersects(const BoundingSphere &bs, float epsilon)
    {
        glm::vec3 cc = mCenter - bs.center();
        glm::vec3::value_type dist = mRadius + bs.radius() - epsilon;

        return (glm::length2(cc) <= dist*dist);
    }

    template <>
    DataReader &DataReader::operator >> <BoundingSphere>(BoundingSphere &bs)
    {
        glm::vec3 center;
        float radius;

        (*this) >> center
                >> radius;

        bs = BoundingSphere(center, radius);

        return *this;
    }


}

