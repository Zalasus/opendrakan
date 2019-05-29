/*
 * LightHandleImpl.cpp
 *
 *  Created on: 17 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/LightHandleImpl.h>

#include <odCore/Light.h>
#include <odCore/LevelObject.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/BulletPhysicsSystem.h>

namespace odBulletPhysics
{

    LightHandle::LightHandle(od::Light &light, btCollisionWorld *collisionWorld)
    : mLight(light)
    , mCollisionWorld(collisionWorld)
    {
        mShape = std::make_unique<btSphereShape>(mLight.getRadius());

        mCollisionObject = std::make_unique<btCollisionObject>();
        mCollisionObject->setCollisionShape(mShape.get());
        mCollisionObject->setUserPointer(static_cast<odPhysics::Handle*>(this));
        mCollisionObject->setCustomDebugColor(btVector3(215.0/256, 221.0/256, 86.0/256));

        btTransform worldTransform = BulletAdapter::makeBulletTransform(mLight.getPosition(), glm::quat(1, 0, 0, 0));
        mCollisionObject->setWorldTransform(worldTransform);

        mCollisionWorld->addCollisionObject(mCollisionObject.get(), odPhysics::PhysicsTypeMasks::Light, odPhysics::PhysicsTypeMasks::All);
    }

    LightHandle::~LightHandle()
    {
        mCollisionObject->setUserPointer(nullptr);
        mCollisionWorld->removeCollisionObject(mCollisionObject.get());
    }

    void LightHandle::setRadius(float radius)
    {
        mShape->setUnscaledRadius(radius);

        mCollisionWorld->updateSingleAabb(mCollisionObject.get());
    }

    void LightHandle::setPosition(const glm::vec3 &pos)
    {
        btTransform worldTransform = BulletAdapter::makeBulletTransform(pos, glm::quat(1, 0, 0, 0));
        mCollisionObject->setWorldTransform(worldTransform);

        mCollisionWorld->updateSingleAabb(mCollisionObject.get());
    }

    od::Light &LightHandle::getLight()
    {
        return mLight;
    }

}
