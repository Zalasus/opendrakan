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

    DefaultObjectClass::~DefaultObjectClass()
    {
    }

    void DefaultObjectClass::probeFields(FieldProbe &probe)
    {
    }

    void DefaultObjectClass::onSpawned(od::LevelObject &obj)
    {
        // create render node if applicable
        odRender::Renderer *renderer = obj.getLevel().getEngine().getRenderer();
        if(renderer != nullptr && obj.getClass()->hasModel())
        {
            mRenderHandle = renderer->createHandleFromObject(obj);

            _updateLighting(obj);
        }

        // if we created a rendering handle, create physics handle, too
        odPhysics::PhysicsSystem &ps = obj.getLevel().getEngine().getPhysicsSystem();
        if(obj.getClass()->hasModel())
        {
            bool hasCollision = obj.getClass()->getModel()->getModelBounds().getShapeCount() != 0;
            mPhysicsHandle = ps.createObjectHandle(obj, !hasCollision);
        }

        mLightReceiver = std::make_unique<od::ObjectLightReceiver>(ps, mPhysicsHandle, mRenderHandle);
    }

    void DefaultObjectClass::onDespawned(od::LevelObject &obj)
    {
        mRenderHandle = nullptr;
        mPhysicsHandle = nullptr;
        mLightReceiver = nullptr;
    }

    void DefaultObjectClass::onMoved(od::LevelObject &obj)
    {
        _updateLighting(obj);
    }

    void DefaultObjectClass::_updateLighting(od::LevelObject &obj)
    {
        if(mRenderHandle != nullptr)
        {
            od::Layer *lightingLayer = (obj.getLightSourceLayer()!= nullptr) ?
                    obj.getLightSourceLayer() : obj.getLevel().getFirstLayerBelowPoint(obj.getPosition());
            if(lightingLayer != nullptr)
            {
                mRenderHandle->setGlobalLight(lightingLayer->getLightDirection(), lightingLayer->getLightColor(), lightingLayer->getAmbientColor());
            }
        }

        if(mLightReceiver != nullptr)
        {
            mLightReceiver->updateAffectingLights();
        }
    }

}



