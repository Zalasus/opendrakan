/*
 * ObjectHandleImpl.h
 *
 *  Created on: Feb 11, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_OBJECTHANDLEIMPL_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_OBJECTHANDLEIMPL_H_

#include <memory>

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include <odCore/physics/Handles.h>

namespace od
{
    class LevelObject;
}

namespace odBulletPhysics
{

    class ObjectHandle : public odPhysics::ObjectHandle
    {
    public:

        ObjectHandle(od::LevelObject &obj, btCollisionWorld *collisionWorld);
        virtual ~ObjectHandle();

        virtual void setEnableCollision(bool b) override;


    private:

        od::LevelObject &mLevelObject;
        btCollisionWorld *mCollisionWorld;

        std::unique_ptr<btCollisionObject> mCollisionObject;
    };

}


#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_OBJECTHANDLEIMPL_H_ */
