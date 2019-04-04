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
#include <odCore/render/ObjectNode.h>
#include <odCore/render/ModelNode.h>

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
            mRenderNode = renderer->createObjectNode(obj);
            if(mRenderNode != nullptr)
            {
                od::RefPtr<odRender::ModelNode> model = obj.getClass()->getModel()->getOrCreateRenderNode(renderer);
                mRenderNode->setModel(model);

                mRenderNode->setPosition(obj.getPosition());
                mRenderNode->setOrientation(obj.getRotation());
                mRenderNode->setScale(obj.getScale());

                mRenderNode->setVisible(obj.isVisible());

                od::Layer *lightingLayer = obj.getLightSourceLayer();
                if(lightingLayer != nullptr)
                {
                    mRenderNode->setGlobalLight(lightingLayer->getLightColor(), lightingLayer->getAmbientColor(), lightingLayer->getLightDirection());
                }
            }
        }

        // if we created a rendering handle, create collisionless physics handle, too. this way, lighting will still work on those models.
        odPhysics::PhysicsSystem &ps = obj.getLevel().getEngine().getPhysicsSystem();
        if(mRenderNode != nullptr)
        {
            mPhysicsHandle = ps.createObjectHandle(obj, true);
        }
    }

    void DefaultObjectClass::onDespawned(od::LevelObject &obj)
    {
        mRenderNode = nullptr;
        mPhysicsHandle = nullptr;
    }

}



