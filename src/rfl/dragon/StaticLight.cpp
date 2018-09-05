/*
 * StaticLight.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#include "rfl/dragon/StaticLight.h"

#include <osg/Light>

#include "rfl/Rfl.h"
#include "LevelObject.h"
#include "Engine.h"

namespace odRfl
{

    StaticLight::StaticLight()
    : mColor(0xffffffff)
    , mIntensityScaling(1.0)
    , mRadius(1.0)
    , mLightMap(od::AssetRef::NULL_REF)
    , mQualityLevelRequired(0)
    {
    }

    void StaticLight::probeFields(RflFieldProbe &probe)
    {
        probe("Point Light")
                (mColor, "Color")
                (mIntensityScaling, "Intensity Scaling")
                (mRadius, "Radius (lu)")
                (mLightMap, "Light Map")
                (mQualityLevelRequired, "Quality Level Required");
    }

    void StaticLight::onSpawned(od::LevelObject &obj)
    {
        if(mLightHandle == nullptr)
        {
            mLightHandle = new od::LightHandle(&obj, mRadius, mQualityLevelRequired);
        }
        obj.getLevel().getEngine().getLightManager().addLight(mLightHandle);

        osg::Light *light = mLightHandle->getLight();
        light->setDiffuse(mColor.asColorVector());
        light->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 0.0));
        light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    }

    void StaticLight::onDespawned(od::LevelObject &obj)
    {
        if(mLightHandle != nullptr)
        {
            obj.getLevel().getEngine().getLightManager().removeLight(mLightHandle);
        }
    }

    OD_REGISTER_RFL_CLASS(0x84, "Static Light", StaticLight);

}

