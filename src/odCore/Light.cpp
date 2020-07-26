/*
 * Light.cpp
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#include <odCore/Light.h>

namespace od
{

    Light::Light()
    : mRadius(1)
    , mIntensityScaling(1)
    , mRequiredQualityLevel(0)
    , mIsDynamic(false)
    {
    }

    bool Light::affects(const glm::vec3 &point)
    {
        return distanceToPoint(point) <= mRadius;
    }

    bool Light::affects(const od::BoundingSphere &sphere)
    {
        return distanceToPoint(sphere.center()) <= (mRadius + sphere.radius());
    }

    float Light::distanceToPoint(const glm::vec3 &point)
    {
        return glm::length(mPosition - point);
    }

}
