/*
 * BulletPhysicsSystem.cpp
 *
 *  Created on: 8 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>

#include <odCore/LevelObject.h>
#include <odCore/Layer.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/LayerHandleImpl.h>
#include <odCore/physics/bullet/ObjectHandleImpl.h>
#include <odCore/physics/bullet/ModelShapeImpl.h>

namespace odBulletPhysics
{

    BulletPhysicsSystem::BulletPhysicsSystem()
    {
        mBroadphase = std::make_unique<btDbvtBroadphase>();
        mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
        mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());

        mCollisionWorld = std::make_unique<btCollisionWorld>(mDispatcher.get(), mBroadphase.get(), mCollisionConfiguration.get());

        // so we get ghost object interaction
        mGhostPairCallback = std::make_unique<btGhostPairCallback>();
        mCollisionWorld->getPairCache()->setInternalGhostPairCallback(mGhostPairCallback.get());
    }

    BulletPhysicsSystem::~BulletPhysicsSystem()
    {
    }

    size_t BulletPhysicsSystem::raycast(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::RayTestResultVector &resultsOut)
    {
        btVector3 bStart = BulletAdapter::toBullet(from);
        btVector3 bEnd =  BulletAdapter::toBullet(to);
        btCollisionWorld::AllHitsRayResultCallback callback(bStart, bEnd);
        mCollisionWorld->rayTest(bStart, bEnd, callback);

        size_t hitObjectCount = callback.m_collisionObjects.size();
        resultsOut.reserve(hitObjectCount);
        for(size_t i = 0; i < hitObjectCount; ++i)
        {
            const btCollisionObject *hitObject = callback.m_collisionObjects[i];
            glm::vec3 hitPoint = BulletAdapter::toGlm(callback.m_hitPointWorld[i]);
            glm::vec3 hitNormal = BulletAdapter::toGlm(callback.m_hitNormalWorld[i]);

            // determine hit object
            if(hitObject->getBroadphaseHandle()->m_collisionFilterGroup == BulletCollisionGroups::LAYER)
            {
                auto it = mLayerHandles.find(hitObject->getUserIndex());
                if(it == mLayerHandles.end() || it->second == nullptr)
                {
                    continue;
                }

                // TODO: for efficiency, we might use the user pointer here instead (won't be typesafe, though)

                od::RefPtr<odPhysics::LayerHandle> layerHandle(it->second.get());
                resultsOut.emplace_back(hitPoint, hitNormal, layerHandle);

            }else if(hitObject->getBroadphaseHandle()->m_collisionFilterGroup == BulletCollisionGroups::OBJECT)
            {
                auto it = mObjectHandles.find(hitObject->getUserIndex());
                if(it == mObjectHandles.end() || it->second == nullptr)
                {
                    continue;
                }

                od::RefPtr<odPhysics::ObjectHandle> objectHandle(it->second.get());
                resultsOut.emplace_back(hitPoint, hitNormal, objectHandle);

            }else
            {
                throw od::Exception("Unexpected collision object type found during ray test");
            }
        }

        return hitObjectCount;
    }

    bool BulletPhysicsSystem::raycastClosest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::Handle *exclude, odPhysics::RayTestResult &resultOut)
    {
        return false; // TODO: implement
    }

    od::RefPtr<odPhysics::ObjectHandle> BulletPhysicsSystem::createObjectHandle(od::LevelObject &obj)
    {
        auto objectHandle = od::make_refd<ObjectHandle>(*this, obj, mCollisionWorld.get());

        mObjectHandles[obj.getObjectId()] = objectHandle;

        return objectHandle.get();
    }

    od::RefPtr<odPhysics::LayerHandle> BulletPhysicsSystem::createLayerHandle(od::Layer &layer)
    {
        auto layerHandle = od::make_refd<LayerHandle>(layer, mCollisionWorld.get());

        mLayerHandles[layer.getId()] = layerHandle;

        return layerHandle.get();
    }

    od::RefPtr<odPhysics::ModelShape> BulletPhysicsSystem::createModelShape(odDb::Model &model)
    {
        auto mb = od::make_refd<ModelShape>(model.getModelBounds());

        return mb.get();
    }

    BulletCollisionGroups::Masks BulletPhysicsSystem::_toBulletMask(odPhysics::PhysicsTypeMasks::Mask mask)
    {
        switch(mask)
        {
        case odPhysics::PhysicsTypeMasks::Layer:
            return BulletCollisionGroups::LAYER;

        case odPhysics::PhysicsTypeMasks::LevelObject:
            return BulletCollisionGroups::OBJECT;

        case odPhysics::PhysicsTypeMasks::Detector:
            return BulletCollisionGroups::DETECTOR;

        default:
            throw od::Exception("Unknown physics type mask");
        }
    }
}
