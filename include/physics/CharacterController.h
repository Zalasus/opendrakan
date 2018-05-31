/*
 * CharacterController.h
 *
 *  Created on: May 23, 2018
 *      Author: zal
 */

#ifndef INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_
#define INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_

#include <memory>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <osg/Vec3f>

namespace od
{

	class LevelObject;
	class PhysicsManager;

	class CharacterController
	{
	public:

		CharacterController(LevelObject &charObject, float radius, float height);

		bool canWalkInDirection(const osg::Vec3f &direction);
		bool canJump();

		void update();


	private:

		LevelObject &mCharObject;
		PhysicsManager &mPhysicsManager;
		std::unique_ptr<btCapsuleShape> mCharShape;
		std::unique_ptr<btPairCachingGhostObject> mGhostObject;
		std::unique_ptr<btKinematicCharacterController> mBtController;

	};

}

#endif /* INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_ */
