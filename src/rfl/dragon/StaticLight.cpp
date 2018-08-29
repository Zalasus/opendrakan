/*
 * StaticLight.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#include "rfl/dragon/StaticLight.h"

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
        mLightHandle = obj.getLevel().getEngine().getLightManager().addLight(&obj);
        if(mLightHandle == nullptr)
        {
            Logger::error() << "Static Light failed to add light object to the scene";
            return;
        }

        mLightHandle->setDiffuseColor(mColor.asColorVector());
        mLightHandle->setIntensity(mIntensityScaling);
        mLightHandle->setRadius(mRadius);
    }


    OD_REGISTER_RFL_CLASS(0x84, "Static Light", StaticLight);

}

