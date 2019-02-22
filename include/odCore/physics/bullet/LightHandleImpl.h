/*
 * LightHandleImpl.h
 *
 *  Created on: 17 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_LIGHTHANDLEIMPL_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_LIGHTHANDLEIMPL_H_

#include <memory>

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>

#include <odCore/physics/Handles.h>

namespace od
{
    class Light;
}

namespace odBulletPhysics
{

    class LightHandle : public odPhysics::LightHandle
    {
    public:

        LightHandle(od::Light *light, btCollisionWorld *collisionWorld);
        virtual ~LightHandle();

        inline btCollisionObject *getBulletObject() { return mCollisionObject.get(); }

        virtual void setRadius(float radius) override;
        virtual void setPosition(const glm::vec3 &pos) override;

        virtual od::Light *getLight() override;


    private:

        od::RefPtr<od::Light> mLight;
        btCollisionWorld *mCollisionWorld;

        std::unique_ptr<btSphereShape> mShape;
        std::unique_ptr<btCollisionObject> mCollisionObject;

    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_LIGHTHANDLEIMPL_H_ */
