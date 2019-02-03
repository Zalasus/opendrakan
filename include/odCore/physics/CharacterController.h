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

#include <odCore/anim/MotionAccumulator.h>

#include <odCore/physics/BulletAdapter.h>

namespace od
{
    class LevelObject;
}

namespace odPhysics
{

	class PhysicsManager;

	class CharacterController : public odAnim::MotionAccumulator
	{
	public:

		CharacterController(od::LevelObject &charObject, float radius, float height);

		// implement odAnim::MotionAccumulator
        virtual void moveRelative(const glm::vec3 &relTranslation, float relTime) override;

		void update(float relTime);


	private:

		bool _step(bool up); // returns true if object was hit during stepping
		bool _needsCollision(const btCollisionObject *body0, const btCollisionObject *body1);

		od::LevelObject &mCharObject;
		btScalar mRadius;
		btScalar mHeight;
		PhysicsManager &mPhysicsManager;
		std::unique_ptr<btCapsuleShape> mCharShape;
		std::unique_ptr<btPairCachingGhostObject> mGhostObject;
		btVector3 mCurrentPosition;
		btScalar mStepHeight;
		btVector3 mUp;
		btVector3 mRelativeLowPoint;
		btVector3 mRelativeHighPoint;
		btVector3 mVelocity;
		btVector3 mDesiredDirection;
		btVector3 mDesiredPosition;

		btManifoldArray mManifoldArray;

		bool mIsFalling;
		float mFallingVelocity;
	};

}

#endif /* INCLUDE_PHYSICS_CHARACTERCONTROLLER_H_ */
