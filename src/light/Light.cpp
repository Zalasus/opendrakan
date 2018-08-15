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

    Light::Light(LevelObject *obj)
    : mLevelObject(obj)
    , mIntensity(1.0)
    , mRadius(1.0)
    {
    }

}

