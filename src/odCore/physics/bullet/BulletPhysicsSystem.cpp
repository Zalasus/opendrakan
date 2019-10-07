/*
 * BulletPhysicsSystem.cpp
 *
 *  Created on: 8 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <odCore/Downcast.h>

#include <odCore/LevelObject.h>
#include <odCore/Layer.h>
#include <odCore/Engine.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/LayerHandleImpl.h>
#include <odCore/physics/bullet/ObjectHandleImpl.h>
#include <odCore/physics/bullet/LightHandleImpl.h>
#include <odCore/physics/bullet/ModelShapeImpl.h>
#include <odCore/physics/bullet/BulletCallbacks.h>
#include <odCore/physics/bullet/DebugDrawer.h>

namespace odBulletPhysics
{

    BulletPhysicsSystem::BulletPhysicsSystem(odRender::Renderer *renderer)
    {
        mBroadphase = std::make_unique<btDbvtBroadphase>();
        mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
        mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfiguration.get());

        mCollisionWorld = std::make_unique<btCollisionWorld>(mDispatcher.get(), mBroadphase.get(), mCollisionConfiguration.get());

        // so we get ghost object interaction
        //mGhostPairCallback = std::make_unique<btGhostPairCallback>();
        //mCollisionWorld->getPairCache()->setInternalGhostPairCallback(mGhostPairCallback.get());

        if(renderer != nullptr)
        {
            mDebugDrawer = std::make_unique<DebugDrawer>(renderer, mCollisionWorld.get());
        }
    }

    BulletPhysicsSystem::~BulletPhysicsSystem()
    {
    }

    size_t BulletPhysicsSystem::rayTest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::RayTestResultVector &resultsOut)
    {
        btVector3 bStart = BulletAdapter::toBullet(from);
        btVector3 bEnd =  BulletAdapter::toBullet(to);

        AllRayCallback callback(bStart, bEnd, typeMask, resultsOut);
        mCollisionWorld->rayTest(bStart, bEnd, callback);

        return callback.getHitCount();
    }

    bool BulletPhysicsSystem::rayTestClosest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::Handle *exclude, odPhysics::RayTestResult &resultOut)
    {
        btVector3 bStart = BulletAdapter::toBullet(from);
        btVector3 bEnd =  BulletAdapter::toBullet(to);

        ClosestRayCallback callback(bStart, bEnd, typeMask, exclude, resultOut);
        mCollisionWorld->rayTest(bStart, bEnd, callback);

        return callback.hasHit();
    }

    size_t BulletPhysicsSystem::contactTest(odPhysics::Handle *handle, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::ContactTestResultVector &resultsOut)
    {
        btCollisionObject *bulletObject;
        switch(handle->getHandleType())
        {
        case odPhysics::Handle::Type::Object:
            bulletObject = od::confident_downcast<ObjectHandle>(handle)->getBulletObject();
            break;

        case odPhysics::Handle::Type::Layer:
            bulletObject = od::confident_downcast<LayerHandle>(handle)->getBulletObject();
            break;

        case odPhysics::Handle::Type::Light:
            bulletObject = od::confident_downcast<LightHandle>(handle)->getBulletObject();
            break;

        default:
             throw od::Exception("Got physics handle of unknown type");
        }

        if(bulletObject == nullptr)
        {
            throw od::Exception("Handle for contact test contained nullptr bullet object");
        }

        ContactResultCallback callback(handle, typeMask, resultsOut);
        mCollisionWorld->contactTest(bulletObject, callback);

        return callback.getContactCount();
    }

    od::RefPtr<odPhysics::ObjectHandle> BulletPhysicsSystem::createObjectHandle(od::LevelObject &obj, bool isDetector)
    {
        auto objectHandle = od::make_refd<ObjectHandle>(*this, obj, mCollisionWorld.get(), isDetector);

        return objectHandle.get();
    }

    od::RefPtr<odPhysics::LayerHandle> BulletPhysicsSystem::createLayerHandle(od::Layer &layer)
    {
        auto layerHandle = od::make_refd<LayerHandle>(layer, mCollisionWorld.get());

        return layerHandle.get();
    }

    od::RefPtr<odPhysics::LightHandle> BulletPhysicsSystem::createLightHandle(od::Light &light)
    {
        auto lightHandle = od::make_refd<LightHandle>(light, mCollisionWorld.get());

        return lightHandle.get();
    }

    od::RefPtr<odPhysics::ModelShape> BulletPhysicsSystem::createModelShape(odDb::Model &model)
    {
        auto mb = od::make_refd<ModelShape>(model.getModelBounds()); // TODO: do we really have to consider LODs here?
        return mb.get();
    }

    void BulletPhysicsSystem::setEnableDebugDrawing(bool enable)
    {
        if(mDebugDrawer == nullptr)
        {
            return;
        }

        int debugDrawMode = enable ? btIDebugDraw::DBG_DrawWireframe : btIDebugDraw::DBG_NoDebug;

        mDebugDrawer->setDebugMode(debugDrawMode);
    }

    bool BulletPhysicsSystem::isDebugDrawingEnabled()
    {
        if(mDebugDrawer == nullptr)
        {
            return false;
        }

        return mDebugDrawer->getDebugMode() != btIDebugDraw::DBG_NoDebug;
    }

    void BulletPhysicsSystem::update(float relTime)
    {
        if(mDebugDrawer != nullptr)
        {
            mDebugDrawer->update(relTime);
        }
    }

}
