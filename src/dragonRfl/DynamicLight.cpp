/*
 * DynamicLight.cpp
 *
 *  Created on: 22 Sep 2018
 *      Author: zal
 */

#include <dragonRfl/DynamicLight.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/LevelObject.h>

namespace od
{

    DynamicLight::DynamicLight(DragonRfl &rfl)
    : StaticLight(rfl)
    , mIntensityEffect(IntensityEffect::Off)
    , mEffectTime(1.0)
    , mEffectAmplitude(1.0)
    , mStartEffect(EffectStartType::WhenCreated)
    , mLightIsOn(true)
    , mStarted(false)
    , mColorFactor(1.0)
    , mPulseRising(false)
    {
    }

    void DynamicLight::probeFields(FieldProbe &probe)
    {
        StaticLight::probeFields(probe);

        probe("Lighting Effects")
                (mIntensityEffect, "Intensity Effect")
                (mEffectTime, "Effect Time (s)")
                (mEffectAmplitude, "Effect Amplitude")
                (mStartEffect, "Start Effect..");

        // this should prevent dividing by zero
        if(mEffectTime <= 0.0)
        {
            mEffectTime = std::numeric_limits<float>::epsilon();
        }
    }

    void DynamicLight::onSpawned(LevelObject &obj)
    {
        StaticLight::onSpawned(obj);

        obj.setEnableRflUpdateHook(true);

        mStarted = (mStartEffect == EffectStartType::WhenCreated);
    }

    void DynamicLight::onUpdate(LevelObject &obj, double simTime, double relTime)
    {
        if(!mStarted)
        {
            return;
        }

        switch(mIntensityEffect)
        {
        case IntensityEffect::Pulse:
            mColorFactor += (mPulseRising ? 1.0 : -1.0) * relTime/mEffectTime;
            if(mPulseRising && mColorFactor > 1.0)
            {
                mColorFactor = 1.0;
                mPulseRising = false;

            }else if(!mPulseRising && mColorFactor < 0.0)
            {
                mColorFactor = 0.0;
                mPulseRising = true;
            }
            break;

        case IntensityEffect::Fade:
            if(mColorFactor > 0.0)
            {
                mColorFactor -= relTime/mEffectTime;
                if(mColorFactor < 0.0)
                {
                    mColorFactor = 0.0;
                }
            }
            break;

        case IntensityEffect::Off:
        default:
            break;
        }

        if(mLightIsOn)
        {
            mLight->setColor(mLightColorVector*mColorFactor);

        }else
        {
            mLight->setColor(osg::Vec3(0.0, 0.0, 0.0));
        }
    }

    void DynamicLight::onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message)
    {
        switch(message)
        {
        case RflMessage::Triggered:
            if(mStartEffect == EffectStartType::WhenEnabled)
            {
                mStarted = true;
            }
            break;

        case RflMessage::On:
            mLightIsOn = true;
            break;

        case RflMessage::Off:
            mLightIsOn = false;
            break;

        default:
            break;
        }
    }


    OD_REGISTER_RFLCLASS(DragonRfl, DynamicLight);

}
