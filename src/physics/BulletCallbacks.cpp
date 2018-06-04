/*
 * BulletCallbacks.cpp
 *
 *  Created on: Jun 4, 2018
 *      Author: zal
 */

#include "physics/BulletCallbacks.h"

namespace od
{

    ClosestNotMeRayResultCallback::ClosestNotMeRayResultCallback(const btVector3 &from, const btVector3 &to, btCollisionObject *me)
    : btCollisionWorld::ClosestRayResultCallback(from, to)
    , mMe(me)
    {
        if(mMe != nullptr)
        {
            m_collisionFilterGroup = mMe->getBroadphaseHandle()->m_collisionFilterGroup;
            m_collisionFilterMask = mMe->getBroadphaseHandle()->m_collisionFilterMask;
        }
    }

    btScalar ClosestNotMeRayResultCallback::addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
    {
        if(rayResult.m_collisionObject == mMe)
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


