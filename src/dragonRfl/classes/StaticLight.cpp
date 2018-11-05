/*
 * StaticLight.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/StaticLight.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/Engine.h>

namespace dragonRfl
{

    StaticLight::StaticLight(DragonRfl &rfl)
    : mColor(0xffffffff)
    , mIntensityScaling(1.0)
    , mRadius(1.0)
    , mLightMap(odDb::AssetRef::NULL_REF)
    , mQualityLevelRequired(0)
    {
    }

    void StaticLight::probeFields(odRfl::FieldProbe &probe)
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
    }

    void StaticLight::onMoved(od::LevelObject &obj)
    {
        static bool warned = false;
        if(!warned)
        {
            Logger::warn() << "Moved light! This is currently unsupported";
            Logger::warn() << "  If light moved into new objects, those will not receive new light until they move themselves";
            warned = true;
        }
    }

    void StaticLight::onDespawned(od::LevelObject &obj)
    {
    }


    OD_REGISTER_RFLCLASS(DragonRfl, StaticLight);

}

