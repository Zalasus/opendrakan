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

#include <glm/vec3.hpp>

#include <odCore/Light.h>

#include <odCore/physics/Handles.h>

namespace dragonRfl
{

    class StaticLight : public odRfl::LevelObjectClassBase
    {
    public:

        StaticLight();

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onUpdate(float relTime) override;
        virtual void onDespawned() override;


    protected:

        odRfl::Color        mColor;
        odRfl::Float        mIntensityScaling;
        odRfl::Float        mRadius;
        odRfl::TextureRef   mLightMap;
        odRfl::Enum         mQualityLevelRequired;

        glm::vec3 mLightColorVector;
        od::RefPtr<od::Light> mLight;
        bool mNeedsUpdate;

    };

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::StaticLight, 0x0084, "Light Source", "Static Light");

#endif /* INCLUDE_RFL_DRAGON_STATICLIGHT_H_ */
