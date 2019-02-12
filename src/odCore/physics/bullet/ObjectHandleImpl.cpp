/*
 * ObjectHandleImpl.cpp
 *
 *  Created on: Feb 11, 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/ObjectHandleImpl.h>

#include <odCore/LevelObject.h>

#include <odCore/physics/bullet/BulletAdapter.h>

namespace odBulletPhysics
{

    ObjectHandle::ObjectHandle(od::LevelObject &obj, btCollisionWorld *collisionWorld)
    : mLevelObject(obj)
    , mCollisionWorld(collisionWorld)
    {
        mCollisionObject = std::make_unique<btCollisionObject>();
        mCollisionObject->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
        // TODO: decide on static/kinematic object depending on user choice/ object type

        btCollisionShape *shape;
        if(mLevelObject.isScaled())
        {
            mUniqueShape = bounds.createUniqueShape();
            shape = mUniqueShape.get();

        }else
        {
            shape = bounds.getSharedShape();
        }
        mCollisionObject->setCollisionShape(shape);

        btTransform transform = BulletAdapter::makeBulletTransform(obj.getPosition(), obj.getRotation());
        mCollisionObject->setWorldTransform(transform);
    }

    ObjectHandle::~ObjectHandle()
    {
        mCollisionWorld->removeCollisionObject(mCollisionObject.get());
    }

    void ObjectHandle::setEnableCollision(bool collisionEnable)
    {
        int cf = mCollisionObject->getCollisionFlags();

        if(collisionEnable)
        {
            cf &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;

        }else
        {
            cf |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
        }

        mCollisionObject->setCollisionFlags(cf);
    }

    void ObjectHandle::setPosition(const glm::vec3 &p)
    {
        btTransform newTransform(mCollisionObject->getWorldTransform());
        newTransform.setOrigin(BulletAdapter::toBullet(p));
        mCollisionObject->setWorldTransform(newTransform);
    }

    void ObjectHandle::setOrientation(const glm::quat &q)
    {
        btTransform newTransform(mCollisionObject->getWorldTransform());
        newTransform.setRotation(BulletAdapter::toBullet(q));
        mCollisionObject->setWorldTransform(newTransform);
    }

    void ObjectHandle::setScale(const glm::vec3 &s)
    {
        // we have to handle scaling differently, as it has to be baked into the collision shape.
        //  so if we are still using the shared shape, make it unique before applying scaling
        if(mUniqueShape == nullptr)
        {
            mUniqueShape = bounds.createUniqueShape();
            mCollisionObject->setCollisionShape(mUniqueShape.get());
        }
        mUniqueShape->setLocalScaling(BulletAdapter::toBullet(s));
    }

}


