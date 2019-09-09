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

    class DynamicLight : public StaticLight
    {
    public:

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

        DynamicLight();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onSpawned() override;
        virtual void onUpdate(float relTime);
        virtual void onTransformChanged() override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    protected:

        odRfl::EnumImpl<IntensityEffect, 0, 4>  mIntensityEffect;
        odRfl::Float                            mEffectTime;
        odRfl::Float                            mEffectAmplitude;
        odRfl::EnumImpl<EffectStartType, 0, 1>  mStartEffect;

        bool mLightIsOn;
        bool mStarted;
        float mColorFactor;
        bool mPulseRising;
    };

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::DynamicLight, 0x0085, "Light Source", "Dynamic Light");

#endif /* INCLUDE_DRAGONRFL_DYNAMICLIGHT_H_ */
