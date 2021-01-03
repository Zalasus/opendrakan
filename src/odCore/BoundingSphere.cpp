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

    bool BoundingSphere::contains(const glm::vec3 &v) const
    {
        return glm::length(mCenter - v) <= mRadius;
    }

    void BoundingSphere::expandBy(const glm::vec3 &v)
    {
        glm::vec3::value_type d = glm::length(mCenter - v);

        if(d > mRadius)
        {
            mRadius = d;
        }
    }

    template <>
    void DataReader::readTyped<BoundingSphere>(BoundingSphere &bs)
    {
        glm::vec3 center;
        float radius;

        (*this) >> center
                >> radius;

        bs = BoundingSphere(center, radius);
    }


}
