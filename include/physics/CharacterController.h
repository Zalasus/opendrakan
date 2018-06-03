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

#include "physics/BulletAdapter.h"
#include "anim/TransformAccumulator.h"

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

	class CharacterController : public od::TransformAccumulator
	{
	public:

		CharacterController(LevelObject &charObject, float radius, float height);

		inline CharacterState getCharacterState() const { return mCharacterState; }
		inline void setVelocity(const osg::Vec3f &v) { mVelocity = BulletAdapter::toBullet(v); }

		// implement od::TransformAccumulator
        virtual void moveRelative(const osg::Vec3f &v);

		void update(double dt);


	private:

		bool _step(bool up); // returns true if object was hit during stepping
		bool _hasInvalidPenetrations();
		bool _needsCollision(const btCollisionObject *body0, const btCollisionObject *body1);

		LevelObject &mCharObject;
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
