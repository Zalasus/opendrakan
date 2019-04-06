/*
 * Light.cpp
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#include <odCore/Light.h>

#include <odCore/Layer.h>
#include <odCore/LevelObject.h>
#include <odCore/LightCallback.h>

#include <odCore/physics/PhysicsSystem.h>
#include <odCore/physics/Handles.h>

namespace od
{

    Light::Light(odPhysics::PhysicsSystem &physicsSystem)
    : mPhysicsSystem(physicsSystem)
    , mRadius(1)
    , mIntensityScaling(1)
    , mRequiredQualityLevel(0)
    , mIsDynamic(false)
    {
        mLightHandle = mPhysicsSystem.createLightHandle(*this);
        mLightHandle->setRadius(mRadius);
    }

    Light::~Light()
    {
    }

    void Light::setPosition(const glm::vec3 &p)
    {
        mPosition = p;
        mLightHandle->setPosition(p);
    }

    void Light::setRadius(float f)
    {
        mRadius = f;
        mLightHandle->setRadius(f);
    }

    bool Light::affects(const glm::vec3 &point)
    {
        return distanceToPoint(point) <= mRadius;
    }

    bool Light::affects(const od::BoundingSphere &sphere)
    {
        return distanceToPoint(sphere.center()) <= (mRadius + sphere.radius());
    }

    float Light::distanceToPoint(const glm::vec3 &point)
    {
        return glm::length(mPosition - point);
    }

    void Light::addAffected(odPhysics::Handle *handle)
    {
        if(handle == nullptr)
        {
            return;
        }

        LightCallback *callback = handle->getLightCallback();
        if(callback != nullptr)
        {
            callback->addAffectingLight(this);
            mAffectedCallbacks.push_back(callback);
        }
    }

    void Light::updateAffectedList()
    {
        for(auto &o : mAffectedCallbacks)
        {
            o->removeAffectingLight(this);
        }
        mAffectedCallbacks.clear();

        static const odPhysics::PhysicsTypeMasks::Mask mask = odPhysics::PhysicsTypeMasks::LevelObject | odPhysics::PhysicsTypeMasks::Layer;

        odPhysics::ContactTestResultVector results;
        mPhysicsSystem.contactTest(mLightHandle, mask, results);

        for(auto &result : results)
        {
            addAffected(result.handle);
        }
    }

}



