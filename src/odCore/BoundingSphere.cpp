/*
 * BoundingSphere.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: zal
 */

#include <odCore/BoundingSphere.h>

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

    template <>
    DataReader &DataReader::operator >> <BoundingSphere>(BoundingSphere &bs)
    {
        glm::vec3 center;
        float radius;

        (*this) >> center
                >> radius;

        return *this;
    }


}


