/*
 * DefaultObjectClass.cpp
 *
 *  Created on: Apr 2, 2019
 *      Author: zal
 */

#include <odCore/rfl/DefaultObjectClass.h>

#include <odCore/Engine.h>
#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Layer.h>

#include <odCore/db/Model.h>
#include <odCore/db/ModelBounds.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Model.h>

#include <odCore/physics/PhysicsSystem.h>
#include <odCore/physics/Handles.h>

namespace odRfl
{

    static void _assignLayerLight(odRender::Handle *handle, od::Layer *layer)
    {
        std::lock_guard<std::mutex> lock(handle->getMutex());
        if(layer != nullptr)
        {
            handle->setGlobalLight(layer->getLightDirection(), layer->getLightColor(), layer->getAmbientColor());

        }else
        {
            handle->setGlobalLight(glm::vec3(1,0,0), glm::vec3(0), glm::vec3(0));
        }
    }

    DefaultObjectClass::DefaultObjectClass()
    {
    }

    DefaultObjectClass::~DefaultObjectClass()
    {
    }

    void DefaultObjectClass::probeFields(FieldProbe &probe)
    {
    }

    void DefaultObjectClass::onSpawned()
    {
        od::LevelObject &obj = getLevelObject();

        // create render node if applicable
        odRender::Renderer *renderer = obj.getLevel().getEngine().getRenderer();
        if(renderer != nullptr && obj.getClass()->hasModel())
        {
            mRenderHandle = renderer->createHandleFromObject(obj);

            od::Layer *lightingLayer = obj.getLightSourceLayer();
            if(lightingLayer == nullptr) lightingLayer = obj.getAssociatedLayer();
            _assignLayerLight(mRenderHandle, lightingLayer);
        }

        // if we created a rendering handle, create physics handle, too
        odPhysics::PhysicsSystem &ps = obj.getLevel().getEngine().getPhysicsSystem();
        if(obj.getClass()->hasModel())
        {
            bool hasCollision = obj.getClass()->getModel()->getModelBounds().getShapeCount() != 0;
            mPhysicsHandle = ps.createObjectHandle(obj, !hasCollision);

            mLightReceiver = std::make_unique<od::ObjectLightReceiver>(ps, mPhysicsHandle, mRenderHandle);
            mLightReceiver->updateAffectingLights();
        }
    }

    void DefaultObjectClass::onDespawned()
    {
        mRenderHandle = nullptr;
        mPhysicsHandle = nullptr;
        mLightReceiver = nullptr;
    }

    void DefaultObjectClass::onTransformChanged()
    {
        od::LevelObject &obj = getLevelObject();

        if(mLightReceiver != nullptr)
        {
            mLightReceiver->updateAffectingLights();
        }

        if(mRenderHandle != nullptr)
        {
            std::lock_guard<std::mutex> lock(mRenderHandle->getMutex());
            mRenderHandle->setPosition(obj.getPosition());
            mRenderHandle->setOrientation(obj.getRotation());
            mRenderHandle->setScale(obj.getScale());
        }

        if(mPhysicsHandle != nullptr)
        {
            mPhysicsHandle->setPosition(obj.getPosition());
            mPhysicsHandle->setOrientation(obj.getRotation());
            mPhysicsHandle->setScale(obj.getScale());
        }
    }

    void DefaultObjectClass::onVisibilityChanged()
    {
        if(mRenderHandle != nullptr)
        {
            std::lock_guard<std::mutex> lock(mRenderHandle->getMutex());
            mRenderHandle->setVisible(getLevelObject().isVisible());
        }

        if(mPhysicsHandle != nullptr)
        {
            mPhysicsHandle->setEnableCollision(getLevelObject().isVisible());
        }
    }

    void DefaultObjectClass::onLayerChanged(od::Layer *from, od::Layer *to)
    {
        if(mRenderHandle != nullptr && getLevelObject().getLightSourceLayer() == nullptr)
        {
            _assignLayerLight(mRenderHandle, to);
        }
    }

}



