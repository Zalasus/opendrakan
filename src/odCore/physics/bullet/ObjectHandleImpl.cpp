/*
 * ObjectHandleImpl.cpp
 *
 *  Created on: Feb 11, 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/ObjectHandleImpl.h>

#include <odCore/LevelObject.h>
#include <odCore/Downcast.h>

#include <odCore/db/Class.h>
#include <odCore/db/Model.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/BulletPhysicsSystem.h>
#include <odCore/physics/bullet/ModelShapeImpl.h>

namespace odBulletPhysics
{

    ObjectHandle::ObjectHandle(BulletPhysicsSystem &ps, od::LevelObject &obj, btCollisionWorld *collisionWorld, bool isDetector)
    : mLevelObject(obj)
    , mCollisionWorld(collisionWorld)
    {
        mCollisionObject = std::make_unique<btCollisionObject>();
        mCollisionObject->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
        // TODO: decide on static/kinematic object depending on user choice/ object type

        std::shared_ptr<odDb::Model> model = obj.getModel();
        if(model == nullptr)
        {
            OD_PANIC() << "Created physics handle for object without model";
        }
        std::shared_ptr<odPhysics::ModelShape> shapeIface = ps.getOrCreateModelShape(model);
        mModelShape = od::confident_downcast<ModelShape>(shapeIface);

        btCollisionShape *bulletShape;
        if(mLevelObject.isScaled())
        {
            mUniqueShape = mModelShape->createNewUniqueShape();
            mUniqueShape->setLocalScaling(BulletAdapter::toBullet(mLevelObject.getScale()));
            bulletShape = mUniqueShape.get();

        }else
        {
            bulletShape = mModelShape->getSharedShape();
        }
        mCollisionObject->setCollisionShape(bulletShape);
        mCollisionObject->setUserPointer(static_cast<Handle*>(this));
        mCollisionObject->setUserIndex(obj.getObjectId());

        btTransform transform = BulletAdapter::makeBulletTransform(obj.getPosition(), obj.getRotation());
        mCollisionObject->setWorldTransform(transform);

        if(isDetector)
        {
            mCollisionObject->setCustomDebugColor(btVector3(86.0/256, 86.0/256, 211.0/256));

        }else
        {
            mCollisionObject->setCustomDebugColor(btVector3(99.0/256, 99.0/256, 99.0/256));
        }

        odPhysics::PhysicsTypeMasks::Mask group = isDetector ? odPhysics::PhysicsTypeMasks::Detector : odPhysics::PhysicsTypeMasks::LevelObject;
        mCollisionWorld->addCollisionObject(mCollisionObject.get(), group, odPhysics::PhysicsTypeMasks::All);
    }

    ObjectHandle::~ObjectHandle()
    {
        mCollisionObject->setUserIndex(-1);
        mCollisionObject->setUserPointer(nullptr);
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

        mCollisionWorld->updateSingleAabb(mCollisionObject.get());
    }

    void ObjectHandle::setOrientation(const glm::quat &q)
    {
        btTransform newTransform(mCollisionObject->getWorldTransform());
        newTransform.setRotation(BulletAdapter::toBullet(q));
        mCollisionObject->setWorldTransform(newTransform);

        mCollisionWorld->updateSingleAabb(mCollisionObject.get());
    }

    void ObjectHandle::setScale(const glm::vec3 &s)
    {
        // we have to handle scaling differently, as it has to be baked into the collision shape.
        //  so if we are still using the shared shape, make it unique before applying scaling
        if(mUniqueShape == nullptr)
        {
            mUniqueShape = mModelShape->createNewUniqueShape();
            mCollisionObject->setCollisionShape(mUniqueShape.get());
        }
        mUniqueShape->setLocalScaling(BulletAdapter::toBullet(s));

        mCollisionWorld->updateSingleAabb(mCollisionObject.get());
    }

    od::LevelObject &ObjectHandle::getLevelObject()
    {
        return mLevelObject;
    }

}
