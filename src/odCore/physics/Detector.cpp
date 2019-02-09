/*
 * Detector.cpp
 *
 *  Created on: 14 Oct 2018
 *      Author: zal
 */

#include <odCore/physics/bullet/Detector.h>

#include <odCore/physics/bullet/PhysicsManager.h>
#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/ModelBounds.h>
#include <odCore/Exception.h>
#include <odCore/LevelObject.h>

namespace odPhysics
{

    Detector::Detector(btCollisionWorld *collisionWorld, std::shared_ptr<ModelCollisionShape> objectShape, od::LevelObject &obj)
    : mCollisionWorld(collisionWorld)
    , mObjectShape(objectShape)
    , mLevelObject(obj)
    , mGhostObject(std::make_unique<btPairCachingGhostObject>())
    {
        mGhostObject->setCollisionShape(mObjectShape.get());
        mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        mGhostObject->setWorldTransform(BulletAdapter::makeBulletTransform(mLevelObject.getPosition(), mLevelObject.getRotation()));

        mCollisionWorld->addCollisionObject(mGhostObject.get(), CollisionGroups::DETECTOR, CollisionGroups::OBJECT);
    }

    Detector::~Detector()
    {
        mCollisionWorld->removeCollisionObject(mGhostObject.get());
    }

    void Detector::update()
    {
        // FIXME: this does not, and can not account for scaling, as bullet does not support scaling a shared collision shape
        mGhostObject->setWorldTransform(BulletAdapter::makeBulletTransform(mLevelObject.getPosition(), mLevelObject.getRotation()));

        btVector3 minAabb;
        btVector3 maxAabb;
        mObjectShape->getAabb(mGhostObject->getWorldTransform(), minAabb, maxAabb);
        mCollisionWorld->getBroadphase()->setAabb(mGhostObject->getBroadphaseHandle(), minAabb, maxAabb, mCollisionWorld->getDispatcher());
        mCollisionWorld->getDispatcher()->dispatchAllCollisionPairs(mGhostObject->getOverlappingPairCache(), mCollisionWorld->getDispatchInfo(), mCollisionWorld->getDispatcher());

        btBroadphasePairArray &pairArray = mGhostObject->getOverlappingPairCache()->getOverlappingPairArray();

        mIntersectingObjects.clear();
        mIntersectingObjects.reserve(pairArray.size());

        for(int pairIndex = 0; pairIndex < pairArray.size(); ++pairIndex)
        {
            btBroadphasePair *collisionPair = &pairArray[pairIndex];

            btCollisionObject *obj0 = static_cast<btCollisionObject*>(collisionPair->m_pProxy0->m_clientObject);
            btCollisionObject *obj1 = static_cast<btCollisionObject*>(collisionPair->m_pProxy1->m_clientObject);

            if(obj0 == nullptr || obj1 == nullptr)
            {
                continue;
            }

            if(obj0 != mGhostObject.get() && obj1 != mGhostObject.get())
            {
                throw od::Exception("The fuck is this, bullet?");
            }

            btCollisionObject *foreignObject = (obj0 != mGhostObject.get()) ? obj0 : obj1;

            // this should already have been caught by the broadphase since we have an object-only mask, but better be safe than sorry
            if(!(foreignObject->getBroadphaseHandle()->m_collisionFilterGroup & CollisionGroups::OBJECT))
            {
                continue;
            }

            if(collisionPair->m_algorithm == nullptr)
            {
                continue;
            }

            collisionPair->m_algorithm->getAllContactManifolds(mManifoldArray);

            // next, check all contact manifolds to see if the shapes are actually overlapping
            bool contact = false;
            for(int manifoldIndex = 0 ; manifoldIndex < mManifoldArray.size(); ++manifoldIndex)
            {
                btPersistentManifold *manifold = mManifoldArray[manifoldIndex];

                for(int contactIndex = 0; contactIndex < manifold->getNumContacts(); ++contactIndex)
                {
                    if(manifold->getContactPoint(contactIndex).getDistance() < 0.0)
                    {
                        contact = true;
                        break;
                    }
                }

                if(contact)
                {
                    break;
                }
            }

            if(contact && foreignObject->getUserPointer() != nullptr)
            {
                // FIXME: type-unsafe, as many things in bullet. still, not nice
                od::LevelObject *levelObject = static_cast<od::LevelObject*>(foreignObject->getUserPointer());
                mIntersectingObjects.push_back(levelObject);
            }
        }
    }

}
