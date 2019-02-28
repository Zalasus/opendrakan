/*
 * DynamicLight.cpp
 *
 *  Created on: 22 Sep 2018
 *      Author: zal
 */

#include <dragonRfl/classes/DynamicLight.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/LevelObject.h>

namespace dragonRfl
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

    void DynamicLight::probeFields(odRfl::FieldProbe &probe)
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

    void DynamicLight::onSpawned(od::LevelObject &obj)
    {
        StaticLight::onSpawned(obj);

        mLight->setDynamic(true);

        obj.setEnableRflUpdateHook(true);

        mStarted = (mStartEffect == EffectStartType::WhenCreated);
    }

    void DynamicLight::onUpdate(od::LevelObject &obj, float relTime)
    {
        StaticLight::onUpdate(obj, relTime);

        if(mLight == nullptr || !mStarted)
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
            mLight->setColor(glm::vec3(0.0, 0.0, 0.0));
        }
    }

    void DynamicLight::onMoved(od::LevelObject &obj)
    {
        mNeedsUpdate = true;
    }

    void DynamicLight::onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message)
    {
        switch(message)
        {
        case odRfl::RflMessage::Triggered:
            if(mStartEffect == EffectStartType::WhenEnabled)
            {
                mStarted = true;
            }
            break;

        case odRfl::RflMessage::On:
            mLightIsOn = true;
            break;

        case odRfl::RflMessage::Off:
            mLightIsOn = false;
            break;

        default:
            break;
        }
    }


    OD_REGISTER_RFLCLASS(DragonRfl, DynamicLight);

}
