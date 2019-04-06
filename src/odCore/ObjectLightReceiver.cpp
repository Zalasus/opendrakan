/*
 * ObjectLightReceiver.cpp
 *
 *  Created on: Apr 5, 2019
 *      Author: zal
 */

#include <odCore/ObjectLightReceiver.h>

#include <odCore/Light.h>

#include <odCore/physics/Handles.h>
#include <odCore/physics/PhysicsSystem.h>

#include <odCore/render/ObjectNode.h>

#include <odCore/Logger.h>

namespace od
{
    ObjectLightReceiver::ObjectLightReceiver(odPhysics::PhysicsSystem &ps, odPhysics::ObjectHandle *oh, odRender::ObjectNode *on)
    : mPhysicsSystem(ps)
    , mPhysicsHandle(oh)
    , mRenderNode(on)
    {
        if(mPhysicsHandle != nullptr)
        {
            mPhysicsHandle->setLightCallback(this);
        }
    }

    ObjectLightReceiver::~ObjectLightReceiver()
    {
        if(mPhysicsHandle != nullptr)
        {
            mPhysicsHandle->setLightCallback(nullptr);
        }
    }

    void ObjectLightReceiver::removeAffectingLight(od::Light *light)
    {
        if(mRenderNode != nullptr)
        {
            mRenderNode->removeLight(light);
        }
    }

    void ObjectLightReceiver::addAffectingLight(od::Light *light)
    {
        Logger::info() << "aaa";

        if(mRenderNode != nullptr)
        {
            mRenderNode->addLight(light);
        }
    }

    void ObjectLightReceiver::clearLightList()
    {
        if(mRenderNode != nullptr)
        {
            mRenderNode->clearLightList();
        }
    }

    void ObjectLightReceiver::updateAffectingLights()
    {
        if(mPhysicsHandle == nullptr)
        {
            return;
        }

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

