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


	PhysicsManager::PhysicsManager(osg::Group *levelRoot)
	: mLevelRoot(levelRoot)
	, mTickCallback(new PhysicsTickCallback(*this))
	{
		mBroadphase.reset(new btDbvtBroadphase());
		mCollisionConfiguration.reset(new btDefaultCollisionConfiguration());
		mDispatcher.reset(new btCollisionDispatcher(mCollisionConfiguration.get()));
		mConstraintSolver.reset(new btSequentialImpulseConstraintSolver());

		mDynamicsWorld.reset(new btDiscreteDynamicsWorld(mDispatcher.get(), mBroadphase.get(), mConstraintSolver.get(), mCollisionConfiguration.get()));

		// for now, hook physics simulation into update traversal. we might want to put this somewhere else once we do threading
		mLevelRoot->addUpdateCallback(mTickCallback);
	}

	PhysicsManager::~PhysicsManager()
	{
		mLevelRoot->removeUpdateCallback(mTickCallback);
	}

	void PhysicsManager::stepSimulation(double dt)
	{
		mDynamicsWorld->stepSimulation(dt, 5); // FIXME: 5 seems good enough, right? no, define this somewhere
	}

	void PhysicsManager::addLayer(Layer &l)
	{
		btCollisionShape *cs = l.getCollisionShape();
		if(cs == nullptr)
		{
			return;
		}

		btRigidBody::btRigidBodyConstructionInfo info(0, nullptr, cs);
		info.m_startWorldTransform.setOrigin(btVector3(l.getOriginX(), l.getWorldHeightLu(), l.getOriginZ()));
		info.m_friction = 0.8;

		btRigidBody *body = new btRigidBody(info);

		mDynamicsWorld->addRigidBody(body, CollisionGroups::LAYER, CollisionGroups::OBJECT);
	}

	void PhysicsManager::addObject(LevelObject &o)
	{
		if(o.getModel() == nullptr || o.getModel()->getModelBounds() == nullptr)
		{
			return;
		}

		btRigidBody::btRigidBodyConstructionInfo info(0.5, &o, o.getModel()->getModelBounds()->getCollisionShape());

		btRigidBody *body = new btRigidBody(info);

		mDynamicsWorld->addRigidBody(body, CollisionGroups::OBJECT, CollisionGroups::ALL);
	}


}
