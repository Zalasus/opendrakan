/*
 * Light.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#include "light/Light.h"

#include <osg/BoundingSphere>

#include "LevelObject.h"

namespace od
{

    LightHandle::LightHandle(LevelObject *obj, float radius, uint32_t requiredQualityLevel)
    : mLevelObject(obj)
    , mRadius(radius)
    , mRequiredQualityLevel(requiredQualityLevel)
    , mLight(new osg::Light)
    {
    }

    bool LightHandle::affects(const osg::Vec3 &point)
    {
        return distanceToPoint(point) <= mRadius;
    }

    float LightHandle::distanceToPoint(const osg::Vec3 &point)
    {
        return (mLevelObject->getPosition() - point).length();
    }

}

