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
    ObjectLightReceiver::ObjectLightReceiver(odPhysics::PhysicsSystem &ps, std::shared_ptr<odPhysics::ObjectHandle> physicsHandle, std::shared_ptr<odRender::Handle> renderHandle)
    : mPhysicsSystem(ps)
    , mPhysicsHandle(physicsHandle)
    , mRenderHandle(renderHandle)
    {
        OD_CHECK_ARG_NONNULL(physicsHandle);

        mPhysicsHandle->setLightCallback(this);
    }

    ObjectLightReceiver::~ObjectLightReceiver()
    {
        mPhysicsHandle->setLightCallback(nullptr);
    }

    void ObjectLightReceiver::removeAffectingLight(std::shared_ptr<od::Light> light)
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

    void ObjectLightReceiver::addAffectingLight(std::shared_ptr<od::Light> light)
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
        mPhysicsSystem.dispatchLighting(mPhysicsHandle);
    }

}
