/*
 * Light.cpp
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#include <odCore/Light.h>

#include <odCore/LevelObject.h>

namespace od
{

    Light::Light(od::LevelObject &obj)
    : mLevelObject(obj)
    , mIntensityScaling(1)
    , mRadius(1)
    , mRequiredQualityLevel(0)
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
        return glm::length(mLevelObject.getPosition() - point);
    }
}
