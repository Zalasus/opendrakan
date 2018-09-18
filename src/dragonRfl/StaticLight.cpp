/*
 * StaticLight.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#include <dragonRfl/StaticLight.h>

#include <osg/Light>
#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>
#include <odCore/Engine.h>

namespace od
{

    StaticLight::StaticLight()
    : mColor(0xffffffff)
    , mIntensityScaling(1.0)
    , mRadius(1.0)
    , mLightMap(od::AssetRef::NULL_REF)
    , mQualityLevelRequired(0)
    {
    }

    void StaticLight::probeFields(FieldProbe &probe)
    {
        probe("Point Light")
                (mColor, "Color")
                (mIntensityScaling, "Intensity Scaling")
                (mRadius, "Radius (lu)")
                (mLightMap, "Light Map")
                (mQualityLevelRequired, "Quality Level Required");
    }

    void StaticLight::onLoaded(od::LevelObject &obj)
    {
        obj.setObjectType(od::LevelObjectType::Light);
    }

    void StaticLight::onSpawned(od::LevelObject &obj)
    {
        if(mLight == nullptr)
        {
            mLight = new od::Light(&obj);
        }
        obj.getLevel().getEngine().getLightManager().addLight(mLight);

        osg::Vec4 color = mColor.asColorVector();
        mLight->setColor(osg::Vec3(color.x(), color.y(), color.z()));
        mLight->setRadius(mRadius);
        mLight->setIntensityScaling(mIntensityScaling);
        mLight->setRequiredQualityLevel(mQualityLevelRequired);
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
    }

    void StaticLight::onDespawned(od::LevelObject &obj)
    {
        if(mLight != nullptr)
        {
            obj.getLevel().getEngine().getLightManager().removeLight(mLight);
        }
    }

    OD_REGISTER_RFL_CLASS(DragonRfl, 0x0084, "Light Source", "Static Light", StaticLight);

}

