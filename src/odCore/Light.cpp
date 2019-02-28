/*
 * Light.cpp
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#include <odCore/Light.h>

#include <odCore/Layer.h>
#include <odCore/LevelObject.h>
#include <odCore/LightReceiver.h>

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

        if(handle->asObjectHandle() != nullptr)
        {
            LevelObject &obj = handle->asObjectHandle()->getLevelObject();
            obj.addAffectingLight(this);
            mAffectedObjects.push_back(&obj);

        }else if(handle->asLayerHandle() != nullptr)
        {
            Layer &layer = handle->asLayerHandle()->getLayer();
            layer.addAffectingLight(this);
            mAffectedLayers.push_back(&layer);
        }
    }

    void Light::updateAffectedList()
    {
        for(auto &l : mAffectedLayers)
        {
            l->removeAffectingLight(this);
        }
        mAffectedLayers.clear();

        for(auto &o : mAffectedObjects)
        {
            o->removeAffectingLight(this);
        }
        mAffectedObjects.clear();

        static const odPhysics::PhysicsTypeMasks::Mask mask = odPhysics::PhysicsTypeMasks::LevelObject | odPhysics::PhysicsTypeMasks::Layer;

        odPhysics::ContactTestResultVector results;
        mPhysicsSystem.contactTest(mLightHandle, mask, results);

        for(auto &result : results)
        {
            addAffected(result.handle);
        }
    }

}



