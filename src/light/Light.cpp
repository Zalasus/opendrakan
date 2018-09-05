/*
 * Light.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#include "light/Light.h"

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

}

