/*
 * StaticLight.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/StaticLight.h>

#include <odCore/Client.h>
#include <odCore/LevelObject.h>

#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{

    StaticLightFields::StaticLightFields()
    : color(255, 255, 255)
    , intensityScaling(1.0)
    , radius(1.0)
    , lightMap(odDb::AssetRef::NULL_REF)
    , qualityLevelRequired(0)
    {
    }

    void StaticLightFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Point Light")
                (color, "Color")
                (intensityScaling, "Intensity Scaling")
                (radius, "Radius (lu)")
                (lightMap, "Light Map")
                (qualityLevelRequired, "Quality Level Required");
    }



    void StaticLight_Cl::onLoaded()
    {
    }

    void StaticLight_Cl::onSpawned()
    {
        auto &obj = getLevelObject();

        glm::vec4 color = mFields.color.asColorVector();

        od::Light lightPrototype;
        lightPrototype.setRadius(mFields.radius);
        lightPrototype.setColor({color.r, color.g, color.b});
        lightPrototype.setIntensityScaling(mFields.intensityScaling);
        lightPrototype.setRequiredQualityLevel(mFields.qualityLevelRequired);
        lightPrototype.setPosition(obj.getPosition());
        lightPrototype.setDynamic(false);

        mLightHandle = getClient().getPhysicsSystem().createLightHandle(lightPrototype);
        getClient().getPhysicsSystem().dispatchLighting(mLightHandle);
    }

    void StaticLight_Cl::onDespawned()
    {
        mLightHandle = nullptr;
    }

}
