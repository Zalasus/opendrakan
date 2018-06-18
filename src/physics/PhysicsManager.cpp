/*
 * PhysicsManager.cpp
 *
 *  Created on: 17 Apr 2018
 *      Author: zal
 */

#include "physics/PhysicsManager.h"

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#include "Layer.h"
#include "LevelObject.h"
#include "Level.h"
#include "Exception.h"
#include "rfl/RflClass.h"
#include "physics/BulletAdapter.h"
#include "physics/BulletCallbacks.h"
#include "Engine.h"
#include "Player.h"

namespace od
{

	class PhysicsTickCallback : public osg::NodeCallback
	{
	public:

		PhysicsTickCallback(PhysicsManager &pm)
		: mPhysicsManager(pm)
		, mFirstUpdate(true)
		, mPrevTime(0)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			traverse(node, nv);

			if(nv->getFrameStamp() != nullptr)
			{
				double simTime = nv->getFrameStamp()->getSimulationTime();

				if(mFirstUpdate)
				{
					mPrevTime = simTime; // so we avoid massive jumps at first update
					mFirstUpdate = false;
				}

				mPhysicsManager.stepSimulation(simTime - mPrevTime);

				mPrevTime = simTime;
			}
		}


	private:

		PhysicsManager &mPhysicsManager;
		bool mFirstUpdate;
		double mPrevTime;

	};




	PhysicsManager::PhysicsManager(Level &level, osg::Group *levelRoot)
	: mLevelRoot(levelRoot)
	, mTickCallback(new PhysicsTickCallback(*this))
	, mLevel(level)
	{
		mBroadphase.reset(new btDbvtBroadphase());
		mCollisionConfiguration.reset(new btDefaultCollisionConfiguration());
		mDispatcher.reset(new btCollisionDispatcher(mCollisionConfiguration.get()));
		mConstraintSolver.reset(new btSequentialImpulseConstraintSolver());

		mDynamicsWorld.reset(new btDiscreteDynamicsWorld(mDispatcher.get(), mBroadphase.get(), mConstraintSolver.get(), mCollisionConfiguration.get()));

		mDynamicsWorld->setGravity(btVector3(0, -1, 0));

		// for now, hook physics simulation into update traversal. we might want to put this somewhere else once we do threading
		mLevelRoot->addUpdateCallback(mTickCallback);

		mDebugDrawer.reset(new DebugDrawer(mLevelRoot, mDynamicsWorld.get()));
		mDynamicsWorld->setDebugDrawer(mDebugDrawer.get());
		//mDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

		// so we get ghost object interaction
		mGhostPairCallback.reset(new btGhostPairCallback);
		mDynamicsWorld->getPairCache()->setInternalGhostPairCallback(mGhostPairCallback.get());
	}

	PhysicsManager::~PhysicsManager()
	{
		mLevelRoot->removeUpdateCallback(mTickCallback);
		mDynamicsWorld->setDebugDrawer(nullptr);

		Logger::debug() << "Physics Manager destroyed with " << mLevelObjectMap.size() + mLayerMap.size() << " rigid bodies left";
	}

	void PhysicsManager::stepSimulation(double dt)
	{
		mDynamicsWorld->stepSimulation(dt, 5); // FIXME: 5 seems good enough, right? no, define this somewhere

		if(mLevel.getEngine().getPlayer() != nullptr)
		{
			mDebugDrawer->setCullingSphere(16, mLevel.getEngine().getPlayer()->getPosition());
		}

		mDebugDrawer->step();
	}

	bool PhysicsManager::toggleDebugDraw()
	{
		if(mDebugDrawer == nullptr)
		{
			return false;
		}

		bool prevState = mDebugDrawer->getDebugMode();
		mDebugDrawer->setDebugMode(!prevState);

		return prevState;
	}

	size_t PhysicsManager::raycast(const osg::Vec3f &start, const osg::Vec3f &end, RaycastResultArray &results)
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
	        result.hitPoint = BulletAdapter::toOsg(callback.m_hitPointWorld[i]);
	        result.hitNormal = BulletAdapter::toOsg(callback.m_hitNormalWorld[i]);

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
                    result.hitLevelObject = it->second.first;
                }
	        }

	        results.push_back(result);
	    }

	    return hitObjectCount;
	}

	bool PhysicsManager::raycastClosest(const osg::Vec3f &start, const osg::Vec3f &end, RaycastResult &result, LevelObject *exclude)
	{
	    btCollisionObject *me = nullptr;
	    if(exclude != nullptr)
	    {
	        auto it = mLevelObjectMap.find(exclude->getObjectId());
	        if(it != mLevelObjectMap.end())
	        {
	            me = it->second.second.get();
	        }
	    }

	    btVector3 bStart = BulletAdapter::toBullet(start);
	    btVector3 bEnd = BulletAdapter::toBullet(end);
	    ClosestNotMeRayResultCallback callback(bStart, bEnd, me);
	    mDynamicsWorld->rayTest(bStart, bEnd, callback);

	    result.hitLayer = nullptr;
        result.hitLevelObject = nullptr;
	    if(!callback.hasHit())
	    {
	        return false;
	    }

	    const btCollisionObject *hitObject = callback.m_collisionObject;
        result.hitBulletObject = hitObject;
        result.hitPoint = BulletAdapter::toOsg(callback.m_hitPointWorld);
        result.hitNormal = BulletAdapter::toOsg(callback.m_hitNormalWorld);

        // determine hit object
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
                result.hitLevelObject = it->second.first;
            }
        }

        return true;
	}

	btRigidBody *PhysicsManager::addLayer(Layer &l)
	{
		btCollisionShape *cs = l.getCollisionShape();
		if(cs == nullptr)
		{
			throw Exception("Tried to add layer without collision shape to PhysicsManager");
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

	void PhysicsManager::removeLayer(Layer &l)
	{
		auto it = mLayerMap.find(l.getId());
		if(it != mLayerMap.end())
		{
			mDynamicsWorld->removeRigidBody(it->second.second.get());

			mLayerMap.erase(it);
		}
	}

	btRigidBody *PhysicsManager::addObject(LevelObject &o, float mass)
	{
		if(o.getModel() == nullptr || o.getModel()->getModelBounds() == nullptr || o.getClassInstance() == nullptr)
		{
			throw Exception("Tried to add object without model or collision shape to PhysicsManager");
		}

		btRigidBody::btRigidBodyConstructionInfo info(mass, &o, o.getModel()->getModelBounds()->getCollisionShape());
		o.getModel()->getModelBounds()->getCollisionShape()->calculateLocalInertia(mass, info.m_localInertia);

		ObjectBodyPair objectBodyPair;
		objectBodyPair.second.reset(new btRigidBody(info));

		btRigidBody *bodyPtr = objectBodyPair.second.get(); // since we are moving the pointer into the map, we need to get a non-managed copy before inserting
		bodyPtr->setUserIndex(o.getObjectId());

		mLevelObjectMap[o.getObjectId()] = std::move(objectBodyPair);

		mDynamicsWorld->addRigidBody(bodyPtr, CollisionGroups::OBJECT, CollisionGroups::ALL);

		return bodyPtr;
	}

	void PhysicsManager::removeObject(LevelObject &o)
	{
		auto it = mLevelObjectMap.find(o.getObjectId());
		if(it != mLevelObjectMap.end())
		{
			mDynamicsWorld->removeRigidBody(it->second.second.get());

			mLevelObjectMap.erase(it);
		}
	}


}
