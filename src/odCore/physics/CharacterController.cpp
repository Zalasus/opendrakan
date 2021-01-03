/*
 * CharacterController.cpp
 *
 *  Created on: May 23, 2018
 *      Author: zal
 */

#include <odCore/physics/CharacterController.h>

#include <odCore/LevelObject.h>
#include <odCore/Level.h>

#include <odCore/physics/PhysicsSystem.h>
#include <odCore/physics/Handles.h>

namespace odPhysics
{

    static const float STEP_HEIGHT = 0.07f;


	CharacterController::CharacterController(PhysicsSystem &physicsSystem, std::shared_ptr<ObjectHandle> handle, od::LevelObject &charObject, float radius, float height)
	: mPhysicsSystem(physicsSystem)
    , mCharObject(charObject)
	, mObjectHandle(handle)
	, mUp(0, 1, 0)
	, mVelocity(0, -1, 0)
	, mIsFalling(false)
	, mFallingVelocity(0.0f)
	{
		mCurrentPosition = mCharObject.getPosition();
        mDesiredPosition = mCurrentPosition;

		mRelativeLowPoint = mUp * (radius+height*(-0.5f));
		mRelativeHighPoint = mUp * (radius+height*0.5f);
	}

    void CharacterController::moveRelative(const glm::vec3 &relTranslation, float relTime)
    {
        glm::vec3 moveDirection = mCharObject.getRotation()*relTranslation;
        glm::vec3 moveTarget = moveDirection + mCharObject.getPosition();

        mDesiredDirection = glm::normalize(moveDirection);
        mDesiredPosition = moveTarget;
    }

	void CharacterController::update(float relTime)
	{
	    // ignore slide collisions for now. just implement stepping. this whole thing needs rewriting anyway
        mCurrentPosition = mDesiredPosition;

        _step(STEP_HEIGHT); // step up
        bool onGround = _step(-STEP_HEIGHT); // step down
        mIsFalling = !onGround;
        mFallingVelocity = 0.0f;

        //ContactTestResultVector results;
        //mPhysicsSystem.contactTest(mObjectHandle, PhysicsTypeMasks::LevelObject | PhysicsTypeMasks::Layer, results);

        mCharObject.setPosition(mCurrentPosition);
	}

	bool CharacterController::_step(float stepHeight)
	{
		/*glm::vec3 rayStart = mCurrentPosition + (stepHeight > 0 ? mRelativeHighPoint : mRelativeLowPoint);
		glm::vec3 rayEnd = rayStart + mUp*stepHeight;
		ClosestNotMeRayResultCallback callback(rayStart, rayEnd, CollisionGroups::ALL, mGhostObject.get());
		mPhysicsManager.mDynamicsWorld->rayTest(rayStart, rayEnd, callback);
		if(callback.hasHit() && _needsCollision(callback.m_collisionObject, mGhostObject.get()))
		{
			mCurrentPosition += mUp*callback.m_closestHitFraction*stepHeight;
			return true;

		}else
		{
			mCurrentPosition += mUp*stepHeight;
			return false;
		}*/

		return false;
	}

}
