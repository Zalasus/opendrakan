/*
 * ObjectLightReceiver.cpp
 *
 *  Created on: Apr 5, 2019
 *      Author: zal
 */

#include <odCore/ObjectLightReceiver.h>

#include <algorithm>

#include <odCore/Light.h>
#include <odCore/Exception.h>
#include <odCore/Logger.h>

#include <odCore/physics/Handles.h>
#include <odCore/physics/PhysicsSystem.h>

#include <odCore/render/Handle.h>

namespace od
{
    ObjectLightReceiver::ObjectLightReceiver(odPhysics::PhysicsSystem &ps, odPhysics::ObjectHandle *physicsHandle, odRender::Handle *renderHandle)
    : mPhysicsSystem(ps)
    , mPhysicsHandle(physicsHandle)
    , mRenderHandle(renderHandle)
    {
        if(mPhysicsHandle == nullptr)
        {
            throw od::InvalidArgumentException("Created ObjectLightReceiver with physics handle = nullptr");
        }

        mPhysicsHandle->setLightCallback(this);
    }

    ObjectLightReceiver::~ObjectLightReceiver()
    {
        mPhysicsHandle->setLightCallback(nullptr);
    }

    void ObjectLightReceiver::removeAffectingLight(od::Light *light)
    {
        if(mRenderHandle != nullptr)
        {
            mRenderHandle->removeLight(light);
        }

        auto it = std::find(mAffectingLights.begin(), mAffectingLights.end(), light);
        if(it != mAffectingLights.end())
        {
            mAffectingLights.erase(it);
        }
    }

    void ObjectLightReceiver::addAffectingLight(od::Light *light)
    {
        if(mRenderHandle != nullptr)
        {
            mRenderHandle->addLight(light);
        }

        mAffectingLights.push_back(light);
    }

    void ObjectLightReceiver::clearLightList()
    {
        if(mRenderHandle != nullptr)
        {
            mRenderHandle->clearLightList();
        }

        // TODO: tell lights to remove us from their list of affected objects
        //for(auto &light : mAffectingLights)
        //{
        //}

        mAffectingLights.clear();
    }

    void ObjectLightReceiver::updateAffectingLights()
    {
        this->clearLightList();

        odPhysics::ContactTestResultVector results;

        static const odPhysics::PhysicsTypeMasks::Mask mask = odPhysics::PhysicsTypeMasks::Light;

        mPhysicsSystem.contactTest(mPhysicsHandle, mask, results);

        for(auto &r : results)
        {
            odPhysics::LightHandle *handle = r.handle->asLightHandle();
            if(handle != nullptr)
            {
                Light &light = handle->getLight();
                this->addAffectingLight(&light);
                light.addAffected(handle);
            }
        }
    }

}

