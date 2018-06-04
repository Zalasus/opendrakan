/*
 * PhysicsManager.h
 *
 *  Created on: 17 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_PHYSICS_PHYSICSMANAGER_H_
#define INCLUDE_PHYSICS_PHYSICSMANAGER_H_

#include <memory>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btConstraintSolver.h>
#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
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
			RAYCAST = 0x0001, // bit 0 reserved by bullet for raytests
			LAYER   = 0x0002,
			OBJECT  = 0x0004,
			ALL     = 0xffff
		};
	};

	class Layer;
	class Level;
	class LevelObject;
	class CharacterController;

	struct RaycastResult
    {
        const btCollisionObject *hitBulletObject;

        osg::Vec3f hitPoint;
        osg::Vec3f hitNormal;

        union
        {
            Layer *hitLayer;
            LevelObject *hitLevelObject;
        };
    };

	typedef std::vector<RaycastResult> RaycastResultArray;

	class PhysicsManager
	{
	public:

		friend class CharacterController;

		PhysicsManager(Level &level, osg::Group *levelRoot);
		~PhysicsManager();

		void stepSimulation(double dt);

		bool toggleDebugDraw();

		/**
		 * Casts a ray into the scene, returning all hit objects sorted by distance from start point.
		 *
		 * @returns Number of hit objects.
		 */
		size_t raycast(const osg::Vec3f &start, const osg::Vec3f &end, RaycastResultArray &results);

		bool raycastClosest(const osg::Vec3f &start, const osg::Vec3f &end, RaycastResult &result, LevelObject *exclude = nullptr);

		btRigidBody *addLayer(Layer &l);
		void removeLayer(Layer &l);
		btRigidBody *addObject(LevelObject &o, float mass);
		void removeObject(LevelObject &o);


	private:

		Level &mLevel;
		osg::ref_ptr<osg::Group> mLevelRoot;
		osg::ref_ptr<osg::NodeCallback> mTickCallback;

		// order is important! mDynamicsWorld needs to be initialized last and destroyed first
		std::unique_ptr<btBroadphaseInterface> mBroadphase;
        std::unique_ptr<btCollisionConfiguration> mCollisionConfiguration;
        std::unique_ptr<btCollisionDispatcher> mDispatcher; // depends on mCollisionConfiguration. init after that
        std::unique_ptr<btConstraintSolver> mConstraintSolver;
        std::unique_ptr<btGhostPairCallback> mGhostPairCallback;
        std::unique_ptr<btDynamicsWorld> mDynamicsWorld;

        std::unique_ptr<DebugDrawer> mDebugDrawer;

        typedef std::pair<Layer*, std::unique_ptr<btRigidBody>> LayerBodyPair;
        typedef std::pair<LevelObject*, std::unique_ptr<btRigidBody>> ObjectBodyPair;
        std::map<uint32_t, ObjectBodyPair> mLevelObjectMap;
        std::map<uint32_t, LayerBodyPair> mLayerMap;
	};

}

#endif /* INCLUDE_PHYSICS_PHYSICSMANAGER_H_ */
