/*
 * PhysicsManager.h
 *
 *  Created on: 17 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_PHYSICS_PHYSICSMANAGER_H_
#define INCLUDE_PHYSICS_PHYSICSMANAGER_H_

#include <memory>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btConstraintSolver.h>
#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <osg/Group>
#include <osg/NodeCallback>

#include "physics/DebugDrawer.h"

namespace od
{

	struct CollisionGroups
	{
		enum Masks
		{
			LAYER  = 0x0002,
			OBJECT = 0x004,
			ALL    = 0xffff
		};
	};

	class Layer;
	class LevelObject;

	class PhysicsManager
	{
	public:

		PhysicsManager(osg::Group *levelRoot);
		~PhysicsManager();

		void stepSimulation(double dt);

		void addLayer(Layer &l);
		void addObject(LevelObject &o, float mass);


	private:

		osg::ref_ptr<osg::Group> mLevelRoot;
		osg::ref_ptr<osg::NodeCallback> mTickCallback;

		// order is important! mDynamicsWorld needs to be initialized last and destroyed first
		std::unique_ptr<btBroadphaseInterface> mBroadphase;
        std::unique_ptr<btCollisionConfiguration> mCollisionConfiguration;
        std::unique_ptr<btCollisionDispatcher> mDispatcher; // depends on mCollisionConfiguration. init after that
        std::unique_ptr<btConstraintSolver> mConstraintSolver;
        std::unique_ptr<btDynamicsWorld> mDynamicsWorld;

        std::unique_ptr<DebugDrawer> mDebugDrawer;
	};

}

#endif /* INCLUDE_PHYSICS_PHYSICSMANAGER_H_ */
