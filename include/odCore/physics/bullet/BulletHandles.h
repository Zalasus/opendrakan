/*
 * BulletHandles.h
 *
 *  Created on: 9 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_BULLETHANDLES_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_BULLETHANDLES_H_

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>

#include <odCore/Layer.h>
#include <odCore/LevelObject.h>

#include <odCore/physics/Handles.h>

namespace odBulletPhysics
{

    class ObjectHandle : public odPhysics::ObjectHandle
    {
    public:

        ObjectHandle(od::LevelObject &obj, btCollisionWorld *collisionWorld);

    };


    class LayerHandle : public odPhysics::ObjectHandle
    {
    public:

        LayerHandle(od::Layer &layer, btCollisionWorld *collisionWorld);

    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_BULLETHANDLES_H_ */
