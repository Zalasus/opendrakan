/*
 * DynamicLight.h
 *
 *  Created on: 22 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_DYNAMICLIGHT_H_
#define INCLUDE_DRAGONRFL_DYNAMICLIGHT_H_

#include <dragonRfl/classes/StaticLight.h>

namespace dragonRfl
{

    struct DynamicLightFields : public StaticLightFields
    {
        enum class IntensityEffect
        {
            Off,
            Pulse,
            Flicker,
            Fade,
            GrowAndShrink
        };

        enum class EffectStartType
        {
            WhenCreated,
            WhenEnabled
        };

        DynamicLightFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::EnumImpl<IntensityEffect, 0, 4>  intensityEffect;
        odRfl::Float                            effectTime;
        odRfl::Float                            effectAmplitude;
        odRfl::EnumImpl<EffectStartType, 0, 1>  startEffect;
    };


    class DynamicLight_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<DynamicLight_Cl>
    {
    public:

        DynamicLight_Cl();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onDespawned() override;
        virtual void onUpdate(float relTime) override;
        virtual void onTransformChanged() override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    private:

        DynamicLightFields mFields;

        std::shared_ptr<odPhysics::LightHandle> mLightHandle;

        glm::vec3 mLightColorVector;
        bool  mLightIsOn;
        bool  mStarted;
        float mColorFactor;
        bool  mPulseRising;
    };


    // this is not a client-only class! the server side has to exists, even if it's just a dummy. but the server might
    //  want to move this light around or switch it off or on, so it can't be deleted there.
    using DynamicLightFactory = odRfl::DefaultClassFactory<DynamicLightFields, DynamicLight_Cl, odRfl::DummyClass>;


    OD_DEFINE_CLASS(DynamicLight, 0x0085, "Light Source", "Dynamic Light", DynamicLightFactory);

}

#endif /* INCLUDE_DRAGONRFL_DYNAMICLIGHT_H_ */
