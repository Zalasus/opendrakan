/*
 * PhysicsManager.h
 *
 *  Created on: 17 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_PHYSICS_PHYSICSMANAGER_H_
#define INCLUDE_PHYSICS_PHYSICSMANAGER_H_

#include <memory>
#include <map>
#include <vector>

#include <glm/vec3.hpp>

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/ConstraintSolver/btConstraintSolver.h>
#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionDispatch/btCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>

namespace od
{
    class Layer;
	class Level;
	class LevelObject;
}

namespace odPhysics
{
    class CharacterController;
    class Detector;
    class ModelCollisionShape;

	struct CollisionGroups
	{
		enum Masks
		{
			RAYCAST  = 0x0001, // bit 0 reserved by bullet for raytests
			LAYER    = 0x0002,
			OBJECT   = 0x0004,
			DETECTOR = 0x0008,
			ALL      = 0xffff
		};
	};

	struct RaycastResult
    {
        const btCollisionObject *hitBulletObject;

        glm::vec3 hitPoint;
        glm::vec3 hitNormal;

        // convenience pointers. if a layer or an object is hit, this will be non-null
        od::Layer *hitLayer;
        od::LevelObject *hitLevelObject;
    };

	typedef std::vector<RaycastResult> RaycastResultArray;

	class PhysicsManager
	{
	public:

		friend class CharacterController;

		PhysicsManager(od::Level &level);
		~PhysicsManager();

		void stepSimulation(double dt);

		/**
		 * Casts a ray into the scene, returning all hit objects sorted by distance from start point.
		 *
		 * @returns Number of hit objects.
		 */
		size_t raycast(const glm::vec3 &start, const glm::vec3 &end, RaycastResultArray &results);

		bool raycastClosest(const glm::vec3 &start, const glm::vec3 &end, RaycastResult &result, od::LevelObject *exclude = nullptr, int mask = CollisionGroups::ALL);
		bool raycastClosestLayer(const glm::vec3 &start, const glm::vec3 &end, RaycastResult &result, od::LevelObject *exclude = nullptr)
		{
		    return raycastClosest(start, end, result, exclude, CollisionGroups::LAYER);
		}
		bool raycastClosestObject(const glm::vec3 &start, const glm::vec3 &end, RaycastResult &result, od::LevelObject *exclude = nullptr)
		{
            return raycastClosest(start, end, result, exclude, CollisionGroups::OBJECT);
        }

		btRigidBody *addLayer(od::Layer &l);
		void removeLayer(od::Layer &l);
		btRigidBody *addObject(od::LevelObject &o, float mass);
		void removeObject(od::LevelObject &o);

		Detector *makeDetector(od::LevelObject &obj);


	private:

		struct ObjectRegistration
		{
		    od::LevelObject *object;
		    std::unique_ptr<btRigidBody> rigidBody;
		    std::shared_ptr<ModelCollisionShape> shape;
		};

		od::Level &mLevel;

		// order is important! mDynamicsWorld needs to be initialized last and destroyed first
		std::unique_ptr<btBroadphaseInterface> mBroadphase;
        std::unique_ptr<btCollisionConfiguration> mCollisionConfiguration;
        std::unique_ptr<btCollisionDispatcher> mDispatcher; // depends on mCollisionConfiguration. init after that
        std::unique_ptr<btConstraintSolver> mConstraintSolver;
        std::unique_ptr<btGhostPairCallback> mGhostPairCallback;
        std::unique_ptr<btDynamicsWorld> mDynamicsWorld;

        typedef std::pair<od::Layer*, std::unique_ptr<btRigidBody>> LayerBodyPair;
        std::map<uint32_t, ObjectRegistration> mLevelObjectMap;
        std::map<uint32_t, LayerBodyPair> mLayerMap;
        std::vector<std::unique_ptr<Detector>> mDetectors;
	};

}

#endif /* INCLUDE_PHYSICS_PHYSICSMANAGER_H_ */
