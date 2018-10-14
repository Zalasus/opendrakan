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

        DynamicLight(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onUpdate(od::LevelObject &obj, double simTime, double relTime);
        virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message) override;


    protected:

        odRfl::RflEnumImpl<IntensityEffect, 0, 4>  mIntensityEffect;
        odRfl::RflFloat                            mEffectTime;
        odRfl::RflFloat                            mEffectAmplitude;
        odRfl::RflEnumImpl<EffectStartType, 0, 1>  mStartEffect;

        bool mLightIsOn;
        bool mStarted;
        float mColorFactor;
        bool mPulseRising;
    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0085, "Light Source", "Dynamic Light", dragonRfl::DynamicLight);

#endif /* INCLUDE_DRAGONRFL_DYNAMICLIGHT_H_ */
