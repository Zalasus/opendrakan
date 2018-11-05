/*
 * BoundingSphere.cpp
 *
 *  Created on: Nov 5, 2018
 *      Author: zal
 */

#include <odCore/BoundingSphere.h>

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

}


