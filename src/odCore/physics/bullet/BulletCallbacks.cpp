/*
 * BulletCallbacks.cpp
 *
 *  Created on: Jun 4, 2018
 *      Author: zal
 */

#include <odCore/physics/bullet/BulletCallbacks.h>

#include <odCore/Exception.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/BulletPhysicsSystem.h>

namespace odBulletPhysics
{


    static void _objectToResult(float fraction, const btVector3 &bHitPoint, const btVector3 &bHitNormal, const btCollisionObject *object, odPhysics::RayTestResult &result)
    {
        glm::vec3 hitPoint = BulletAdapter::toGlm(bHitPoint);
        glm::vec3 hitNormal = BulletAdapter::toGlm(bHitNormal);

        // determine hit object
        if(object->getBroadphaseHandle()->m_collisionFilterGroup == odPhysics::PhysicsTypeMasks::Layer)
        {
            od::RefPtr<odPhysics::Handle> handle = static_cast<odPhysics::Handle*>(object->getUserPointer());
            if(handle == nullptr || handle->asLayerHandle() == nullptr)
            {
                throw od::Exception("Hit collision object with layer group which had no layer handle assigned");
            }

            result = odPhysics::RayTestResult(fraction, hitPoint, hitNormal, handle->asLayerHandle());

        }else if(object->getBroadphaseHandle()->m_collisionFilterGroup == odPhysics::PhysicsTypeMasks::LevelObject)
        {
            od::RefPtr<odPhysics::Handle> handle = static_cast<odPhysics::Handle*>(object->getUserPointer());
            if(handle == nullptr || handle->asObjectHandle() == nullptr)
            {
                throw od::Exception("Hit collision object with object group which had no object handle assigned");
            }

            result = odPhysics::RayTestResult(fraction, hitPoint, hitNormal, handle->asObjectHandle());

        }else if(object->getBroadphaseHandle()->m_collisionFilterGroup == odPhysics::PhysicsTypeMasks::Light)
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


    ClosestNotMeConvexResultCallback::ClosestNotMeConvexResultCallback(btCollisionObject *me)
    : btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
    , mMe(me)
    {
        if(mMe != nullptr)
        {
            m_collisionFilterGroup = mMe->getBroadphaseHandle()->m_collisionFilterGroup;
            m_collisionFilterMask = mMe->getBroadphaseHandle()->m_collisionFilterMask;
        }
    }

    btScalar ClosestNotMeConvexResultCallback::addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
    {
        if(convexResult.m_hitCollisionObject == mMe)
        {
            return 1.0;
        }

        if(!convexResult.m_hitCollisionObject->hasContactResponse())
        {
            return 1.0;
        }

        return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
    }


    ClosestRayCallback::ClosestRayCallback(const btVector3 &start, const btVector3 &end, odPhysics::PhysicsTypeMasks::Mask mask, odPhysics::Handle *exclude, odPhysics::RayTestResult &result)
    : mResult(result)
    , mStart(start)
    , mEnd(end)
    , mExclude(exclude)
    {
        m_collisionFilterGroup = odPhysics::PhysicsTypeMasks::Ray;
        m_collisionFilterMask = mask;
    }

    btScalar ClosestRayCallback::addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
    {
        if(m_collisionObject != nullptr)
        {
            return 1.0;
        }

        if(rayResult.m_collisionObject == nullptr)
        {
            throw od::Exception("Got nullptr as collision object");
        }

        if(mExclude != nullptr)
        {
            odPhysics::Handle *userPointerAsHandle = static_cast<odPhysics::Handle*>(rayResult.m_collisionObject->getUserPointer());
            if(mExclude == userPointerAsHandle)
            {
                return rayResult.m_hitFraction;
            }
        }

        m_closestHitFraction = rayResult.m_hitFraction;
        m_collisionObject = rayResult.m_collisionObject;

        btVector3 hitNormal;
        if(normalInWorldSpace)
        {
            hitNormal = rayResult.m_hitNormalLocal;

        } else
        {
            ///need to transform normal into worldspace
            hitNormal = rayResult.m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
        }

        btVector3 hitPoint;
        hitPoint.setInterpolate3(mStart, mEnd, rayResult.m_hitFraction);

        _objectToResult(rayResult.m_hitFraction, hitPoint, hitNormal, rayResult.m_collisionObject, mResult);

        return rayResult.m_hitFraction;
    }


    AllRayCallback::AllRayCallback(const btVector3 &start, const btVector3 &end, odPhysics::PhysicsTypeMasks::Mask mask, odPhysics::RayTestResultVector &results)
    : mResults(results)
    , mStart(start)
    , mEnd(end)
    , mHitCount(0)
    {
        m_collisionFilterGroup = odPhysics::PhysicsTypeMasks::Ray;
        m_collisionFilterMask = mask;
    }

    btScalar AllRayCallback::addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
    {
        if(rayResult.m_collisionObject == nullptr)
        {
            throw od::Exception("Got nullptr as collision object");
        }

        m_closestHitFraction = rayResult.m_hitFraction;
        m_collisionObject = rayResult.m_collisionObject;

        btVector3 hitNormal;
        if(normalInWorldSpace)
        {
            hitNormal = rayResult.m_hitNormalLocal;

        } else
        {
            ///need to transform normal into worldspace
            hitNormal = rayResult.m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
        }

        btVector3 hitPoint;
        hitPoint.setInterpolate3(mStart, mEnd, rayResult.m_hitFraction);

        mResults.emplace_back();
        _objectToResult(rayResult.m_hitFraction, hitPoint, hitNormal, rayResult.m_collisionObject, mResults.back());

        ++mHitCount;

        return rayResult.m_hitFraction;
    }


    ContactResultCallback::ContactResultCallback(odPhysics::PhysicsTypeMasks::Mask mask, odPhysics::ContactTestResultVector &results)
    : mResults(results)
    , mContactCount(0)
    , mLastObject(nullptr)
    {
        m_collisionFilterGroup = odPhysics::PhysicsTypeMasks::Ray; // this can't be right, right?
        m_collisionFilterMask = mask;
    }

    btScalar ContactResultCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
    {
        // for the time being, we don't care about manifolds. only store object pairs

        // colObj0Wrap seems to always be the object used to start the contact test

        if(mLastObject == colObj1Wrap->m_collisionObject)
        {
            return 0.0;

        }else
        {
            mLastObject = colObj1Wrap->m_collisionObject;
        }

        odPhysics::ContactTestResult result;
        result.handleA = static_cast<odPhysics::Handle*>(colObj0Wrap->m_collisionObject->getUserPointer());
        result.handleB = static_cast<odPhysics::Handle*>(colObj1Wrap->m_collisionObject->getUserPointer());
        mResults.push_back(result);

        ++mContactCount;

        return 0.0;
    }

}


