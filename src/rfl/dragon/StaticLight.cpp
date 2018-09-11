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
        light->setPosition(osg::Vec4(obj.getPosition(), 1.0));
        light->setDiffuse(mColor.asColorVector());
        light->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 0.0));
        light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));

        // light (at least static lights) seem like they are not attenuated at all in the riot engine.
        //  their effect solely depends on their radius. however, since the default FFP light model does not allow
        //  for a cutoff radius, having no attenuation would mean a light would illuminate layers and object globally.
        //  for now, attenuate them. later, we could encode the radius in one of the attenuation parameters and do the
        //  cutoff in our shader (would look weird on FFP, but who cares about FFP anyway).
        light->setConstantAttenuation(0.0);
        light->setLinearAttenuation(2.0/mRadius);
        light->setQuadraticAttenuation(0.0);
    }

    void StaticLight::onMoved(od::LevelObject &obj)
    {
        static bool warned = false;
        if(!warned)
        {
            Logger::warn() << "Moved static light! This is currently unsupported";
            Logger::warn() << "  If light moved into new objects, those will not receive new light until they move themselves";
            warned = true;
        }

        mLightHandle->getLight()->setPosition(osg::Vec4(obj.getPosition(), 1.0));
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

