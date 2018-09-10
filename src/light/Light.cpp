/*
 * Light.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#include "light/Light.h"

#include <osg/BoundingSphere>

#include "LevelObject.h"
#include "light/LightManager.h"

namespace od
{

    Light::Light(LightManager &lm, LevelObject *obj)
    : mLightManager(lm)
    , mLevelObject(obj)
    , mColor(osg::Vec3(1.0, 1.0, 1.0))
    , mIntensityScaling(1.0)
    , mRadius(1.0)
    , mRequiredQualityLevel(0)
    {
    }

    bool Light::affects(const osg::Vec3 &point)
    {
        return distanceToPoint(point) <= mRadius;
    }

    bool Light::affects(const osg::BoundingSphere &sphere)
    {
        return distanceToPoint(sphere.center()) <= (mRadius + sphere.radius());
    }

    float Light::distanceToPoint(const osg::Vec3 &point)
    {
        return (mLevelObject->getPosition() - point).length();
    }

    void Light::apply(size_t index)
    {
        mLightManager.applyToLightUniform(this, index);
    }

}

