/*
 * BulletCallbacks.cpp
 *
 *  Created on: Jun 4, 2018
 *      Author: zal
 */

#include <odCore/physics/bullet/BulletCallbacks.h>

#include <odCore/Panic.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/BulletPhysicsSystem.h>

namespace odBulletPhysics
{

    static std::shared_ptr<odPhysics::Handle> _handlePtrFromObject(const btCollisionObject *object)
    {
        // FIXME: it would be great to find an alternative to enable_shared_from_this here
        auto handle = static_cast<odPhysics::Handle*>(object->getUserPointer()); // user pointer is a void*, so we can't use od::downcast here :/
        return handle->shared_from_this();
    }


    static void _objectToResult(float fraction, const btVector3 &bHitPoint, const btVector3 &bHitNormal, const btCollisionObject *object, odPhysics::RayTestResult &result)
    {
        result.hitFraction = fraction;
        result.hitPoint = BulletAdapter::toGlm(bHitPoint);
        result.hitNormal = BulletAdapter::toGlm(bHitNormal);
        result.handle = _handlePtrFromObject(object);
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


    ClosestRayCallback::ClosestRayCallback(const btVector3 &start, const btVector3 &end, odPhysics::PhysicsTypeMasks::Mask mask, std::shared_ptr<odPhysics::Handle> exclude, odPhysics::RayTestResult &result)
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
        m_closestHitFraction = rayResult.m_hitFraction;

        if(rayResult.m_collisionObject == nullptr)
        {
            OD_PANIC() << "Got nullptr as collision object";
        }

        if(mExclude != nullptr)
        {
            odPhysics::Handle *userPointerAsHandle = static_cast<odPhysics::Handle*>(rayResult.m_collisionObject->getUserPointer());
            if(mExclude.get() == userPointerAsHandle)
            {
                return 1.0;
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
            OD_PANIC() << "Got nullptr as collision object";
        }

        m_collisionObject = rayResult.m_collisionObject;

        btVector3 hitNormal;
        if(normalInWorldSpace)
        {
            hitNormal = rayResult.m_hitNormalLocal;

        } else
        {
            // need to transform normal into worldspace
            hitNormal = rayResult.m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
        }

        btVector3 hitPoint;
        hitPoint.setInterpolate3(mStart, mEnd, rayResult.m_hitFraction);

        mResults.emplace_back();
        _objectToResult(rayResult.m_hitFraction, hitPoint, hitNormal, rayResult.m_collisionObject, mResults.back());

        ++mHitCount;

        return m_closestHitFraction;
    }


    ContactResultCallback::ContactResultCallback(btCollisionObject *me, odPhysics::PhysicsTypeMasks::Mask mask, odPhysics::ContactTestResultVector &results)
    : mMe(me)
    , mResults(results)
    , mContactCount(0)
    , mLastObject(nullptr)
    {
        m_collisionFilterGroup = odPhysics::PhysicsTypeMasks::All; // this can't be right, right?
        m_collisionFilterMask = mask;
    }

    btScalar ContactResultCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
    {
        // for the time being, we don't care about manifolds. only store object pairs

        if(mLastObject == colObj0Wrap->m_collisionObject || mLastObject == colObj1Wrap->m_collisionObject)
        {
            return 0.0;
        }

        odPhysics::ContactTestResult result;
        if(colObj0Wrap->m_collisionObject == mMe)
        {
            result.handle = _handlePtrFromObject(colObj1Wrap->m_collisionObject);
            mLastObject = colObj1Wrap->m_collisionObject;

        }else if(colObj1Wrap->m_collisionObject == mMe)
        {
            result.handle = _handlePtrFromObject(colObj0Wrap->m_collisionObject);;
            mLastObject = colObj0Wrap->m_collisionObject;

        }else
        {
            OD_PANIC() << "Contact test yielded manifold where neither of the two pair objects was the initiator";
        }

        mResults.push_back(result);

        ++mContactCount;

        return 0.0;
    }

}
