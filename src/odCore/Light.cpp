/*
 * Light.cpp
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#include <odCore/Light.h>

#include <odCore/LevelObject.h>

#include <odCore/physics/PhysicsSystem.h>

namespace od
{

    Light::Light(odPhysics::PhysicsSystem &physicsSystem)
    : mPhysicsSystem(physicsSystem)
    , mIntensityScaling(1)
    , mRadius(1)
    , mRequiredQualityLevel(0)
    {
        mLightHandle = mPhysicsSystem.createLightHandle(*this);
    }

    void Light::setPosition(const glm::vec3 &p)
    {
        mPosition = p;
        mLightHandle->setPosition(p);
    }

    void Light::setRadius(float f)
    {
        mRadius = f;
        mLightHandle->setRadius(f);
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
