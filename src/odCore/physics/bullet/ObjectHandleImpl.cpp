/*
 * ObjectHandleImpl.cpp
 *
 *  Created on: Feb 11, 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/ObjectHandleImpl.h>

namespace odBulletPhysics
{

    ObjectHandle::ObjectHandle(od::LevelObject &obj, btCollisionWorld *collisionWorld)
    : mLevelObject(obj)
    , mCollisionWorld(collisionWorld)
    {

    }

    ObjectHandle::~ObjectHandle()
    {
        if(mCollisionObject != nullptr)
        {
            mCollisionWorld->removeCollisionObject(mCollisionObject.get());
        }
    }

    void ObjectHandle::setEnableCollision(bool b)
    {
        if(mCollisionObject != nullptr)
        {
        }
    }

}


