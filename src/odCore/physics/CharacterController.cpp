/*
 * CharacterController.cpp
 *
 *  Created on: May 23, 2018
 *      Author: zal
 */

#include <odCore/physics/CharacterController.h>

#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/physics/PhysicsManager.h>
#include <odCore/physics/BulletCallbacks.h>

namespace odPhysics
{

	CharacterController::CharacterController(od::LevelObject &charObject, float radius, float height)
	: mCharObject(charObject)
	, mRadius(radius)
	, mHeight(height)
	, mPhysicsManager(charObject.getLevel().getPhysicsManager())
	, mStepHeight(0.07)
	, mUp(0, 1, 0)
	, mVelocity(0, -1, 0)
	, mIsFalling(false)
	, mFallingVelocity(0.0f)
	{
		mCharShape = std::make_unique<btCapsuleShape>(radius, height);
		mGhostObject = std::make_unique<btPairCachingGhostObject>();
		mGhostObject->setCollisionShape(mCharShape.get());
		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_CHARACTER_OBJECT);
		mGhostObject->setWorldTransform(BulletAdapter::makeBulletTransform(charObject.getPosition(), charObject.getRotation()));
		mPhysicsManager.mDynamicsWorld->addCollisionObject(mGhostObject.get(), CollisionGroups::OBJECT, CollisionGroups::ALL);

		mGhostObject->setUserPointer(&charObject);

		mCurrentPosition = BulletAdapter::toBullet(mCharObject.getPosition());
        mDesiredPosition = mCurrentPosition;

		mRelativeLowPoint = mUp * -(radius+height*0.5);
		mRelativeHighPoint = mUp * (radius+height*0.5);
	}

    void CharacterController::moveRelative(const glm::vec3 &relTranslation, float relTime)
    {
        glm::vec3 moveDirection = mCharObject.getRotation()*relTranslation;
        glm::vec3 moveTarget = moveDirection + mCharObject.getPosition();

        mDesiredDirection = BulletAdapter::toBullet(glm::normalize(moveDirection));
        mDesiredPosition = BulletAdapter::toBullet(moveTarget);
    }

	void CharacterController::update(float relTime)
	{
	    if(!mIsFalling)
	    {
            _step(true); // step up

            // slide as far as possible
            btTransform from;
            from.setIdentity();
            from.setOrigin(mCurrentPosition);

            btTransform to;
            to.setIdentity();
            to.setOrigin(mDesiredPosition);

            ClosestNotMeConvexResultCallback callback(mGhostObject.get());
            mGhostObject->convexSweepTest(mCharShape.get(), from, to, callback, mPhysicsManager.mDynamicsWorld->getDispatchInfo().m_allowedCcdPenetration);
            if(callback.hasHit() && _needsCollision(callback.m_hitCollisionObject, mGhostObject.get()))
            {
                //mCurrentPosition += (callback.m_closestHitFraction - mRadius) * mDesiredDirection;

            }else
            {
                mCurrentPosition = mDesiredPosition;
            }

            bool onGround = _step(false); // step down
            mIsFalling = !onGround;
            mFallingVelocity = 0.0f;

	    }else
	    {
	        // last step had us falling. ignore desired position and simply apply gravity until we hit the ground.
	        //  no stepping is necessary, as we are not moving forward

	        mFallingVelocity = glm::clamp(0.2*relTime + mFallingVelocity, 0.0, 5.0);

	        btVector3 gravityStart  = mCurrentPosition;
	        btVector3 gravityTarget = (mCurrentPosition - mUp*mFallingVelocity*relTime);

            ClosestNotMeRayResultCallback callback(gravityStart, gravityTarget, CollisionGroups::ALL, mGhostObject.get());
            mPhysicsManager.mDynamicsWorld->rayTest(gravityStart, gravityTarget, callback);
            if(callback.hasHit() && _needsCollision(callback.m_collisionObject, mGhostObject.get()))
            {
                _step(false);
                mIsFalling = false;

            }else
            {
                mCurrentPosition = gravityTarget;
            }
	    }

	    mGhostObject->setWorldTransform(btTransform(btQuaternion(0,0,0,1), mCurrentPosition));
        mCharObject.setPosition(BulletAdapter::toGlm(mCurrentPosition));
	}

	bool CharacterController::_step(bool up)
	{
		btVector3 rayStart = mCurrentPosition + (up ? mRelativeHighPoint : mRelativeLowPoint);
		btVector3 rayEnd = rayStart + mUp*(up ? mStepHeight : (-mStepHeight - 0.1));
		ClosestNotMeRayResultCallback callback(rayStart, rayEnd, CollisionGroups::ALL, mGhostObject.get());
		mPhysicsManager.mDynamicsWorld->rayTest(rayStart, rayEnd, callback);
		if(callback.hasHit() && _needsCollision(callback.m_collisionObject, mGhostObject.get()))
		{
			mCurrentPosition += mUp*callback.m_closestHitFraction*mStepHeight*(up ? 1 : -1);
			return true;

		}else
		{
			mCurrentPosition += mUp*mStepHeight*(up ? 1 : -1);
			return false;
		}
	}

	bool CharacterController::_needsCollision(const btCollisionObject *body0, const btCollisionObject *body1)
	{
		bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
		collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);
		return collides;
	}

}


