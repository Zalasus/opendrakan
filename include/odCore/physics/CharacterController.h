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

#include <odCore/physics/BulletAdapter.h>

namespace od
{
    class LevelObject;
}

namespace odPhysics
{

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

		CharacterController(od::LevelObject &charObject, float radius, float height);

		inline CharacterState getCharacterState() const { return mCharacterState; }

		// implement od::TransformAccumulator
        virtual void moveRelative(const glm::vec3 &v);

		void update(double dt);


	private:

		bool _step(bool up); // returns true if object was hit during stepping
		bool _hasInvalidPenetrations();
		bool _needsCollision(const btCollisionObject *body0, const btCollisionObject *body1);

		od::LevelObject &mCharObject;
		btScalar mRadius;
		btScalar mHeight;
		PhysicsManager &mPhysicsManager;
		std::unique_ptr<btCapsuleShape> mCharShape;
		std::unique_ptr<btPairCachingGhostObject> mGhostObject;
		btVector3 mCurrentPosition;
		CharacterState mCharacterState;
		btScalar mStepHeight;
		btVector3 mUp;
		btVector3 mRelativeLowPoint;
		btVector3 mRelativeHighPoint;
		btVector3 mVelocity;
		btVector3 mDesiredDirection;
		btVector3 mDesiredPosition;

		btManifoldArray mManifoldArray;

	};

}

#endif /* INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_ */
