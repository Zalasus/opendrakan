/*
 * StaticLight.h
 *
 *  Created on: Aug 15, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_STATICLIGHT_H_
#define INCLUDE_RFL_DRAGON_STATICLIGHT_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>
#include <odCore/rfl/DummyClass.h>

#include <glm/vec3.hpp>

#include <odCore/Light.h>

#include <odCore/physics/Handles.h>

namespace dragonRfl
{

    struct StaticLightFields : public odRfl::FieldBundle
    {

        StaticLightFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::Color        color;
        odRfl::Float        intensityScaling;
        odRfl::Float        radius;
        odRfl::TextureRef   lightMap;
        odRfl::Enum         qualityLevelRequired;
    };


    class StaticLight_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<StaticLight_Cl>
    {
    public:

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onDespawned() override;


    private:

        StaticLightFields mFields;

        glm::vec3 mLightColorVector;
        std::shared_ptr<odPhysics::LightHandle> mLightHandle;

    };

    using StaticLightFactory = odRfl::DefaultClassFactory<StaticLightFields, StaticLight_Cl, odRfl::DummyClass>;

    OD_DEFINE_CLASS(StaticLight, 0x0084, "Light Source", "Static Light", StaticLightFactory);

}

#endif /* INCLUDE_RFL_DRAGON_STATICLIGHT_H_ */
