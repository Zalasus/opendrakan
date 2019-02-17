/*
 * BulletCallbacks.cpp
 *
 *  Created on: Jun 4, 2018
 *      Author: zal
 */

#include <odCore/physics/bullet/BulletCallbacks.h>

namespace odBulletPhysics
{

    ClosestRayCallback::ClosestRayCallback(const btVector3 &from, const btVector3 &to, int mask, odPhysics::Handle *exclude)
    : btCollisionWorld::ClosestRayResultCallback(from, to)
    , mMask(mask)
    , mExclude(exclude)
    {
        m_collisionFilterMask = mask;
    }

    btScalar ClosestRayCallback::addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
    {
        if(   rayResult.m_collisionObject == nullptr
           || rayResult.m_collisionObject->getUserPointer() == mExclude
           || !(rayResult.m_collisionObject->getBroadphaseHandle()->m_collisionFilterGroup & mMask))
        {
            return 1.0;
        }

        return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
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

}


