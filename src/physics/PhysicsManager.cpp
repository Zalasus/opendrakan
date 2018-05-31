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

		if(mLevel.getPlayer() != nullptr)
		{
			mDebugDrawer->setCullingSphere(16, mLevel.getPlayer()->getPosition());
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

		std::unique_ptr<btRigidBody> body(new btRigidBody(info));
		btRigidBody *bodyPtr = body.get(); // since we are moving the pointer into the map, we need to get a non-managed copy before inserting
		mLayerMap[l.getId()] = std::move(body);

		mDynamicsWorld->addRigidBody(bodyPtr, CollisionGroups::LAYER, CollisionGroups::OBJECT);

		return bodyPtr;
	}

	void PhysicsManager::removeLayer(Layer &l)
	{
		auto it = mLayerMap.find(l.getId());
		if(it != mLayerMap.end())
		{
			mDynamicsWorld->removeRigidBody(it->second.get());

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

		std::unique_ptr<btRigidBody> body(new btRigidBody(info));
		btRigidBody *bodyPtr = body.get(); // since we are moving the pointer into the map, we need to get a non-managed copy before inserting
		mLevelObjectMap[o.getObjectId()] = std::move(body);

		mDynamicsWorld->addRigidBody(bodyPtr, CollisionGroups::OBJECT, CollisionGroups::ALL);

		return bodyPtr;
	}

	void PhysicsManager::removeObject(LevelObject &o)
	{
		auto it = mLevelObjectMap.find(o.getObjectId());
		if(it != mLevelObjectMap.end())
		{
			mDynamicsWorld->removeRigidBody(it->second.get());

			mLevelObjectMap.erase(it);
		}
	}


}
