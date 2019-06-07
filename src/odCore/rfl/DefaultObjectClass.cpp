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
        }

        // if we created a rendering handle, create physics handle, too
        odPhysics::PhysicsSystem &ps = obj.getLevel().getEngine().getPhysicsSystem();
        if(obj.getClass()->hasModel())
        {
            bool hasCollision = obj.getClass()->getModel()->getModelBounds().getShapeCount() != 0;
            mPhysicsHandle = ps.createObjectHandle(obj, !hasCollision);

            mLightReceiver = std::make_unique<od::ObjectLightReceiver>(ps, mPhysicsHandle, mRenderHandle);
        }

        _updateLighting(obj);
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
            od::Layer *lightingLayer;
            if(obj.getLightSourceLayer() != nullptr)
            {
                lightingLayer = obj.getLightSourceLayer();

            }else
            {
                // a slight upwards offset fixes many lighting issues with objects whose origin is exactly on the ground
                glm::vec3 probePoint = obj.getPosition() + glm::vec3(0, 0.1, 0);
                lightingLayer = obj.getLevel().getFirstLayerBelowPoint(probePoint);
            }

            if(lightingLayer != nullptr)
            {
                mRenderHandle->setGlobalLight(lightingLayer->getLightDirection(), lightingLayer->getLightColor(), lightingLayer->getAmbientColor());

            }else
            {
                mRenderHandle->setGlobalLight(glm::vec3(1,0,0), glm::vec3(0), glm::vec3(0));
            }
        }

        if(mLightReceiver != nullptr)
        {
            mLightReceiver->updateAffectingLights();
        }
    }

}



