/*
 * PhysicsManager.cpp
 *
 *  Created on: 17 Apr 2018
 *      Author: zal
 */

#include <odCore/physics/PhysicsManager.h>

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#include <odCore/Layer.h>
#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/Exception.h>
#include <odCore/Engine.h>
#include <odCore/rfl/RflClass.h>
#include <odCore/physics/BulletAdapter.h>
#include <odCore/physics/BulletCallbacks.h>
#include <odCore/physics/Detector.h>

namespace odPhysics
{


	PhysicsManager::PhysicsManager(od::Level &level)
	: mLevel(level)
	{
		mBroadphase.reset(new btDbvtBroadphase());
		mCollisionConfiguration.reset(new btDefaultCollisionConfiguration());
		mDispatcher.reset(new btCollisionDispatcher(mCollisionConfiguration.get()));
		mConstraintSolver.reset(new btSequentialImpulseConstraintSolver());

		mDynamicsWorld.reset(new btDiscreteDynamicsWorld(mDispatcher.get(), mBroadphase.get(), mConstraintSolver.get(), mCollisionConfiguration.get()));

		mDynamicsWorld->setGravity(btVector3(0, -1, 0));

		// so we get ghost object interaction
		mGhostPairCallback.reset(new btGhostPairCallback);
		mDynamicsWorld->getPairCache()->setInternalGhostPairCallback(mGhostPairCallback.get());
	}

	PhysicsManager::~PhysicsManager()
	{
		Logger::debug() << "Physics Manager destroyed with " << mLevelObjectMap.size() + mLayerMap.size() << " rigid bodies left";
	}

	void PhysicsManager::stepSimulation(double dt)
	{
		mDynamicsWorld->stepSimulation(dt, 5); // FIXME: 5 seems good enough, right? no, define this somewhere
	}

	size_t PhysicsManager::raycast(const glm::vec3 &start, const glm::vec3 &end, RaycastResultArray &results)
	{
	    results.clear();

	    btVector3 bStart = BulletAdapter::toBullet(start);
	    btVector3 bEnd =  BulletAdapter::toBullet(end);
	    btCollisionWorld::AllHitsRayResultCallback callback(bStart, bEnd);
	    mDynamicsWorld->rayTest(bStart, bEnd, callback);

	    if(!callback.hasHit())
	    {
	        return 0;
	    }

	    size_t hitObjectCount = callback.m_collisionObjects.size();
	    results.reserve(hitObjectCount);
	    for(size_t i = 0; i < hitObjectCount; ++i)
	    {
	        const btCollisionObject *hitObject = callback.m_collisionObjects[i];

	        RaycastResult result;
	        result.hitBulletObject = hitObject;
	        result.hitPoint = BulletAdapter::toGlm(callback.m_hitPointWorld[i]);
	        result.hitNormal = BulletAdapter::toGlm(callback.m_hitNormalWorld[i]);

	        // determine hit object
	        result.hitLayer = nullptr;
	        result.hitLevelObject = nullptr;
	        if(hitObject->getBroadphaseHandle()->m_collisionFilterGroup & CollisionGroups::LAYER)
	        {
	            auto it = mLayerMap.find(hitObject->getUserIndex());
	            if(it != mLayerMap.end())
	            {
	                result.hitLayer = it->second.first;
	            }

	        }

	        if(hitObject->getBroadphaseHandle()->m_collisionFilterGroup & CollisionGroups::OBJECT)
	        {
	            auto it = mLevelObjectMap.find(hitObject->getUserIndex());
                if(it != mLevelObjectMap.end())
                {
                    result.hitLevelObject = it->second.object;
                }
	        }

	        results.push_back(result);
	    }

	    return hitObjectCount;
	}

	bool PhysicsManager::raycastClosest(const glm::vec3 &start, const glm::vec3 &end, RaycastResult &result, od::LevelObject *exclude, int mask)
	{
	    btCollisionObject *me = nullptr;
	    if(exclude != nullptr)
	    {
	        auto it = mLevelObjectMap.find(exclude->getObjectId());
	        if(it != mLevelObjectMap.end())
	        {
	            me = it->second.rigidBody.get();
	        }
	    }

	    btVector3 bStart = BulletAdapter::toBullet(start);
	    btVector3 bEnd = BulletAdapter::toBullet(end);
	    ClosestNotMeRayResultCallback callback(bStart, bEnd, mask, me);
	    mDynamicsWorld->rayTest(bStart, bEnd, callback);

	    result.hitLayer = nullptr;
        result.hitLevelObject = nullptr;
	    if(!callback.hasHit())
	    {
	        return false;
	    }

	    const btCollisionObject *hitObject = callback.m_collisionObject;
        result.hitBulletObject = hitObject;
        result.hitPoint = BulletAdapter::toGlm(callback.m_hitPointWorld);
        result.hitNormal = BulletAdapter::toGlm(callback.m_hitNormalWorld);

        // if hit object is a layer or an object, set pointers to allow caller quick access
        if(hitObject->getBroadphaseHandle()->m_collisionFilterGroup & CollisionGroups::LAYER)
        {
            auto it = mLayerMap.find(hitObject->getUserIndex());
            if(it != mLayerMap.end())
            {
                result.hitLayer = it->second.first;
            }
        }

        if(hitObject->getBroadphaseHandle()->m_collisionFilterGroup & CollisionGroups::OBJECT)
        {
            auto it = mLevelObjectMap.find(hitObject->getUserIndex());
            if(it != mLevelObjectMap.end())
            {
                result.hitLevelObject = it->second.object;
            }
        }

        return true;
	}

	btRigidBody *PhysicsManager::addLayer(od::Layer &l)
	{
		btCollisionShape *cs = l.getCollisionShape();
		if(cs == nullptr)
		{
			throw od::Exception("Tried to add layer without collision shape to PhysicsManager");
		}

		btRigidBody::btRigidBodyConstructionInfo info(0, nullptr, cs);
		info.m_startWorldTransform.setOrigin(btVector3(l.getOriginX(), l.getWorldHeightLu(), l.getOriginZ()));
		info.m_friction = 0.8;

		LayerBodyPair layerBodyPair;
		layerBodyPair.first = &l;
		layerBodyPair.second.reset(new btRigidBody(info));

		btRigidBody *bodyPtr = layerBodyPair.second.get(); // since we are moving the pointer into the map, we need to get a non-managed copy before inserting
		bodyPtr->setUserIndex(l.getId());

		mLayerMap[l.getId()] = std::move(layerBodyPair);

		mDynamicsWorld->addRigidBody(bodyPtr, CollisionGroups::LAYER, CollisionGroups::OBJECT | CollisionGroups::RAYCAST);

		return bodyPtr;
	}

	void PhysicsManager::removeLayer(od::Layer &l)
	{
		auto it = mLayerMap.find(l.getId());
		if(it != mLayerMap.end())
		{
			mDynamicsWorld->removeRigidBody(it->second.second.get());

			mLayerMap.erase(it);
		}
	}

	btRigidBody *PhysicsManager::addObject(od::LevelObject &obj, float mass)
	{
		if(obj.getClass() == nullptr || obj.getClass()->getModel() == nullptr || obj.getClass()->getModel()->getModelBounds() == nullptr)
		{
			throw od::Exception("Tried to add object without model or collision shape to PhysicsManager");
		}

		ObjectRegistration reg;
		reg.object = &obj;
		if(!obj.isScaled())
		{
		    reg.shape = obj.getClass()->getModel()->getModelBounds()->getSharedCollisionShape();

		}else
		{
		    reg.shape = obj.getClass()->getModel()->getModelBounds()->buildNewShape();
		    reg.shape->setLocalScaling(BulletAdapter::toBullet(obj.getScale()));
		}

		btRigidBody::btRigidBodyConstructionInfo info(mass, &obj, reg.shape);
        reg.shape->calculateLocalInertia(mass, info.m_localInertia);

		reg.rigidBody.reset(new btRigidBody(info));

		btRigidBody *bodyPtr = reg.rigidBody.get(); // since we are moving the pointer into the map, we need to get a non-managed copy before inserting
		bodyPtr->setUserIndex(obj.getObjectId());
		bodyPtr->setUserPointer(&obj);

		mLevelObjectMap[obj.getObjectId()] = std::move(reg);

		mDynamicsWorld->addRigidBody(bodyPtr, CollisionGroups::OBJECT, CollisionGroups::ALL);

		return bodyPtr;
	}

	void PhysicsManager::removeObject(od::LevelObject &o)
	{
		auto it = mLevelObjectMap.find(o.getObjectId());
		if(it != mLevelObjectMap.end())
		{
			mDynamicsWorld->removeRigidBody(it->second.rigidBody.get());

			mLevelObjectMap.erase(it);
		}
	}

	Detector *PhysicsManager::makeDetector(od::LevelObject &obj)
	{
	    if(obj.getClass() == nullptr || obj.getClass()->getModel() == nullptr || obj.getClass()->getModel()->getModelBounds() == nullptr)
        {
            throw od::Exception("Tried to make detector from object without model or collision shape to PhysicsManager");
        }

	    std::shared_ptr<ModelCollisionShape> shape;
	    if(!obj.isScaled())
        {
	        shape = obj.getClass()->getModel()->getModelBounds()->getSharedCollisionShape();

        }else
        {
            shape = obj.getClass()->getModel()->getModelBounds()->buildNewShape();
            shape->setLocalScaling(BulletAdapter::toBullet(obj.getScale()));
        }

	    mDetectors.push_back(std::unique_ptr<Detector>(new Detector(mDynamicsWorld.get(), shape, obj)));
	    return mDetectors.back().get();
	}

}
