/*
 * BulletCallbacks.h
 *
 *  Created on: Jun 4, 2018
 *      Author: zal
 */

#ifndef INCLUDE_PHYSICS_BULLETCALLBACKS_H_
#define INCLUDE_PHYSICS_BULLETCALLBACKS_H_

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>

#include <odCore/physics/Handles.h>

namespace odBulletPhysics
{

    class ClosestRayCallback : public btCollisionWorld::ClosestRayResultCallback
    {
    public:

        ClosestRayCallback(const btVector3 &from, const btVector3 &to, int mask, odPhysics::Handle *exclude);

        virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace);


    protected:

        int mMask;
        od::RefPtr<odPhysics::Handle> mExclude;
    };


    class ClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
    {
    public:

        ClosestNotMeConvexResultCallback(btCollisionObject *me);

        virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace);


    protected:

        btCollisionObject *mMe;
    };

}


#endif /* INCLUDE_PHYSICS_BULLETCALLBACKS_H_ */
