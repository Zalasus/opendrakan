/*
 * LightHandleImpl.cpp
 *
 *  Created on: 17 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/LightHandleImpl.h>

#include <odCore/Exception.h>
#include <odCore/Light.h>
#include <odCore/LevelObject.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/BulletPhysicsSystem.h>

namespace odBulletPhysics
{

    LightHandle::LightHandle(const od::Light &light, btCollisionWorld *collisionWorld)
    : mCollisionWorld(collisionWorld)
    {
        OD_CHECK_ARG_NONNULL(collisionWorld);

        mLight = std::make_shared<od::Light>(light);

        mShape = std::make_unique<btSphereShape>(light.getRadius());

        mCollisionObject = std::make_unique<btCollisionObject>();
        mCollisionObject->setCollisionShape(mShape.get());
        mCollisionObject->setUserPointer(static_cast<odPhysics::Handle*>(this));
        mCollisionObject->setCustomDebugColor(btVector3(215.0/256, 221.0/256, 86.0/256));

        btTransform worldTransform = BulletAdapter::makeBulletTransform(light.getPosition(), glm::quat(1, 0, 0, 0));
        mCollisionObject->setWorldTransform(worldTransform);

        auto flags = light.isDynamic() ? btCollisionObject::CF_KINEMATIC_OBJECT : btCollisionObject::CF_STATIC_OBJECT;
        mCollisionObject->setCollisionFlags(flags);

        mCollisionWorld->addCollisionObject(mCollisionObject.get(), odPhysics::PhysicsTypeMasks::Light, odPhysics::PhysicsTypeMasks::All);
    }

    LightHandle::~LightHandle()
    {
        mCollisionObject->setUserPointer(nullptr);
        mCollisionWorld->removeCollisionObject(mCollisionObject.get());
    }

    void LightHandle::setRadius(float radius, bool modifyLight)
    {
        mShape->setUnscaledRadius(radius);

        mCollisionWorld->updateSingleAabb(mCollisionObject.get());

        if(modifyLight)
        {
            mLight->setRadius(radius);
        }
    }

    void LightHandle::setPosition(const glm::vec3 &pos, bool modifyLight)
    {
        btTransform worldTransform = BulletAdapter::makeBulletTransform(pos, glm::quat(1, 0, 0, 0));
        mCollisionObject->setWorldTransform(worldTransform);

        mCollisionWorld->updateSingleAabb(mCollisionObject.get());

        if(modifyLight)
        {
            mLight->setPosition(pos);
        }
    }

    std::shared_ptr<od::Light> LightHandle::getLight()
    {
        return mLight;
    }

}
