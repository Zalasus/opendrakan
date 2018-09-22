/*
 * DynamicLight.h
 *
 *  Created on: 22 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_DYNAMICLIGHT_H_
#define INCLUDE_DRAGONRFL_DYNAMICLIGHT_H_

#include <dragonRfl/StaticLight.h>

namespace od
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

        virtual void probeFields(FieldProbe &probe) override;

        virtual void onSpawned(LevelObject &obj) override;
        virtual void onUpdate(LevelObject &obj, double simTime, double relTime);
        virtual void onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message) override;


    protected:

        RflEnumImpl<IntensityEffect, 0, 4>  mIntensityEffect;
        RflFloat                            mEffectTime;
        RflFloat                            mEffectAmplitude;
        RflEnumImpl<EffectStartType, 0, 1>  mStartEffect;

        bool mLightIsOn;
        bool mStarted;
        float mColorFactor;
        bool mPulseRising;
    };


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x0085, "Light Source", "Dynamic Light", DynamicLight);

}

#endif /* INCLUDE_DRAGONRFL_DYNAMICLIGHT_H_ */
