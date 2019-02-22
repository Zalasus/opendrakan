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
#include <odCore/physics/bullet/LightHandleImpl.h>
#include <odCore/physics/bullet/ModelShapeImpl.h>
#include <odCore/physics/bullet/BulletCallbacks.h>

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

    size_t BulletPhysicsSystem::rayTest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::RayTestResultVector &resultsOut)
    {
        btVector3 bStart = BulletAdapter::toBullet(from);
        btVector3 bEnd =  BulletAdapter::toBullet(to);

        // TODO: we might want to write a custom callback that handles the mask internally and directly converts to RayTestResult
        btCollisionWorld::AllHitsRayResultCallback callback(bStart, bEnd);
        mCollisionWorld->rayTest(bStart, bEnd, callback);

        int bulletMask = _toBulletMask(typeMask);

        size_t hitObjectCount = callback.m_collisionObjects.size();
        resultsOut.reserve(hitObjectCount);
        for(size_t i = 0; i < hitObjectCount; ++i)
        {
            const btCollisionObject *hitObject = callback.m_collisionObjects[i];

            if(!(hitObject->getBroadphaseHandle()->m_collisionFilterGroup & bulletMask))
            {
                continue;
            }

            resultsOut.emplace_back();

            _objectToResult(callback.m_hitFractions[i], callback.m_hitPointWorld[i], callback.m_hitNormalWorld[i], hitObject, resultsOut.back());
        }

        return hitObjectCount;
    }

    bool BulletPhysicsSystem::rayTestClosest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::Handle *exclude, odPhysics::RayTestResult &resultOut)
    {
        btVector3 bStart = BulletAdapter::toBullet(from);
        btVector3 bEnd =  BulletAdapter::toBullet(to);
        ClosestRayCallback callback(bStart, bEnd, _toBulletMask(typeMask), exclude);
        mCollisionWorld->rayTest(bStart, bEnd, callback);

        if(callback.hasHit())
        {
            _objectToResult(callback.m_closestHitFraction, callback.m_hitPointWorld, callback.m_hitNormalWorld, callback.m_collisionObject, resultOut);

            return true;
        }

        return false;
    }

    size_t BulletPhysicsSystem::contactTest(const glm::vec3 &v)
    {
        return 0;
    }

    od::RefPtr<odPhysics::ObjectHandle> BulletPhysicsSystem::createObjectHandle(od::LevelObject &obj)
    {
        auto objectHandle = od::make_refd<ObjectHandle>(*this, obj, mCollisionWorld.get());

        return objectHandle.get();
    }

    od::RefPtr<odPhysics::LayerHandle> BulletPhysicsSystem::createLayerHandle(od::Layer &layer)
    {
        auto layerHandle = od::make_refd<LayerHandle>(layer, mCollisionWorld.get());

        return layerHandle.get();
    }

    od::RefPtr<odPhysics::LightHandle> BulletPhysicsSystem::createLightHandle(od::Light &light)
    {
        auto lightHandle = od::make_refd<LightHandle>(&light, mCollisionWorld.get());

        return lightHandle.get();
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

        case odPhysics::PhysicsTypeMasks::Light:
            return BulletCollisionGroups::LIGHT;

        case odPhysics::PhysicsTypeMasks::All:
            return BulletCollisionGroups::ALL;

        default:
            throw od::Exception("Unknown physics type mask");
        }
    }

    void BulletPhysicsSystem::_objectToResult(float fraction, const btVector3 &bHitPoint, const btVector3 &bHitNormal, const btCollisionObject *object, odPhysics::RayTestResult &result)
    {
        glm::vec3 hitPoint = BulletAdapter::toGlm(bHitPoint);
        glm::vec3 hitNormal = BulletAdapter::toGlm(bHitNormal);

        // determine hit object
        if(object->getBroadphaseHandle()->m_collisionFilterGroup == BulletCollisionGroups::LAYER)
        {
            od::RefPtr<odPhysics::Handle> handle = static_cast<odPhysics::Handle*>(object->getUserPointer());
            if(handle == nullptr || handle->asLayerHandle() == nullptr)
            {
                throw od::Exception("Hit collision object with layer group which had no layer handle assigned");
            }

            result = odPhysics::RayTestResult(fraction, hitPoint, hitNormal, handle->asLayerHandle());

        }else if(object->getBroadphaseHandle()->m_collisionFilterGroup == BulletCollisionGroups::OBJECT)
        {
            od::RefPtr<odPhysics::Handle> handle = static_cast<odPhysics::Handle*>(object->getUserPointer());
            if(handle == nullptr || handle->asObjectHandle() == nullptr)
            {
                throw od::Exception("Hit collision object with object group which had no object handle assigned");
            }

            result = odPhysics::RayTestResult(fraction, hitPoint, hitNormal, handle->asObjectHandle());

        }else if(object->getBroadphaseHandle()->m_collisionFilterGroup == BulletCollisionGroups::LIGHT)
        {
            od::RefPtr<odPhysics::Handle> handle = static_cast<odPhysics::Handle*>(object->getUserPointer());
            if(handle == nullptr || handle->asLightHandle() == nullptr)
            {
                throw od::Exception("Hit collision object with light group which had no light handle assigned");
            }

            result = odPhysics::RayTestResult(fraction, hitPoint, hitNormal, handle->asLightHandle());

        }else
        {
            throw od::Exception("Unexpected collision object type found during ray test");
        }
    }
}
