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

	enum class CharacterState
	{
		Ok,
		Penetrated_Object,
		Falling
	};

	class CharacterController
	{
	public:

		CharacterController(LevelObject &charObject, float radius, float height);

		bool canWalkInDirection(const osg::Vec3f &direction);
		bool canJump();
		inline CharacterState getCharacterState() const { return mCharacterState; }

		void update(double dt);


	private:

		bool _stepUp();
		bool _stepDown(); // returns true if falling
		bool _hasInvalidPenetrations();
		bool _needsCollision(const btCollisionObject *body0, const btCollisionObject *body1);

		LevelObject &mCharObject;
		PhysicsManager &mPhysicsManager;
		std::unique_ptr<btCapsuleShape> mCharShape;
		std::unique_ptr<btPairCachingGhostObject> mGhostObject;
		btVector3 mCurrentPosition;
		CharacterState mCharacterState;
		btScalar mStepHeight;
		btVector3 mUp;
		btVector3 mGravity;
		btVector3 mRelativeLowPoint;
		btVector3 mVelocity;

		btManifoldArray mManifoldArray;

	};

}

#endif /* INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_ */
