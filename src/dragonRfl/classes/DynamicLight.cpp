/*
 * DynamicLight.cpp
 *
 *  Created on: 22 Sep 2018
 *      Author: zal
 */

#include <dragonRfl/classes/DynamicLight.h>

#include <odCore/Client.h>
#include <odCore/LevelObject.h>

#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{

    DynamicLightFields::DynamicLightFields()
    : intensityEffect(IntensityEffect::Off)
    , effectTime(1.0)
    , effectAmplitude(1.0)
    , startEffect(EffectStartType::WhenCreated)
    {
    }

    void DynamicLightFields::probeFields(odRfl::FieldProbe &probe)
    {
        StaticLightFields::probeFields(probe);

        probe("Lighting Effects")
                (intensityEffect, "Intensity Effect")
                (effectTime, "Effect Time (s)")
                (effectAmplitude, "Effect Amplitude")
                (startEffect, "Start Effect..");

        // this should prevent dividing by zero
        if(effectTime <= 0.0)
        {
            effectTime = std::numeric_limits<float>::epsilon();
        }
    }

    DynamicLight_Cl::DynamicLight_Cl()
    : mLightIsOn(true)
    , mStarted(false)
    , mColorFactor(1.0)
    , mPulseRising(false)
    {
    }

    void DynamicLight_Cl::onLoaded()
    {
    }

    void DynamicLight_Cl::onSpawned()
    {
        auto &obj = getLevelObject();

        glm::vec4 color = mFields.color.asColorVector();
        mLightColorVector = {color.r, color.g, color.b};

        od::Light lightPrototype;
        lightPrototype.setRadius(mFields.radius);
        lightPrototype.setColor(mLightColorVector);
        lightPrototype.setIntensityScaling(mFields.intensityScaling);
        lightPrototype.setRequiredQualityLevel(mFields.qualityLevelRequired);
        lightPrototype.setPosition(obj.getPosition());
        lightPrototype.setDynamic(true);

        mLightHandle = getClient().getPhysicsSystem().createLightHandle(lightPrototype);
        getClient().getPhysicsSystem().dispatchLighting(mLightHandle);

        obj.setEnableUpdate(true);

        mStarted = (mFields.startEffect == DynamicLightFields::EffectStartType::WhenCreated);
    }

    void DynamicLight_Cl::onDespawned()
    {
        mLightHandle = nullptr;
    }

    void DynamicLight_Cl::onUpdate(float relTime)
    {
        if(mLightHandle == nullptr || !mStarted)
        {
            return;
        }

        switch(mFields.intensityEffect)
        {
        case DynamicLightFields::IntensityEffect::Pulse:
            mColorFactor += (mPulseRising ? 1.0 : -1.0) * relTime/mFields.effectTime;
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

        case DynamicLightFields::IntensityEffect::Fade:
            if(mColorFactor > 0.0)
            {
                mColorFactor -= relTime/mFields.effectTime;
                if(mColorFactor < 0.0)
                {
                    mColorFactor = 0.0;
                }
            }
            break;

        case DynamicLightFields::IntensityEffect::Off:
        default:
            break;
        }

        if(mLightIsOn)
        {
            mLightHandle->getLight()->setColor(mLightColorVector*mColorFactor);

        }else
        {
            mLightHandle->getLight()->setColor(glm::vec3(0.0, 0.0, 0.0)); // TODO: lights need an off-flag. an off-state light could free up one light slot
        }
    }

    void DynamicLight_Cl::onTransformChanged()
    {
        getClient().getPhysicsSystem().dispatchLighting(mLightHandle);
    }

    void DynamicLight_Cl::onMessageReceived(od::LevelObject &sender, od::Message message)
    {
        switch(message)
        {
        case od::Message::Triggered:
            if(mFields.startEffect == DynamicLightFields::EffectStartType::WhenEnabled)
            {
                mStarted = true;
            }
            break;

        case od::Message::On:
            mLightIsOn = true;
            break;

        case od::Message::Off:
            mLightIsOn = false;
            break;

        default:
            break;
        }
    }

}
