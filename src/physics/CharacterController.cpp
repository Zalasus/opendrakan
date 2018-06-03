/*
 * CharacterController.cpp
 *
 *  Created on: May 23, 2018
 *      Author: zal
 */

#include "physics/CharacterController.h"

#include "LevelObject.h"
#include "Level.h"
#include "physics/PhysicsManager.h"
#include "physics/BulletAdapter.h"

namespace od
{


	class ClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
	public:

		ClosestNotMeRayResultCallback(const btVector3 &from, const btVector3 &to, btCollisionObject *me)
		: btCollisionWorld::ClosestRayResultCallback(from, to)
		, mMe(me)
		{
		}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			if(rayResult.m_collisionObject == mMe)
			{
				return 1.0;
			}

			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}

	protected:

		btCollisionObject *mMe;
	};

	class ClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
	{
	public:

		ClosestNotMeConvexResultCallback(btCollisionObject *me)
		: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
		, mMe(me)
		{
		}

		virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			if(convexResult.m_hitCollisionObject == mMe)
			{
				return 1.0;
			}

			if(!convexResult.m_hitCollisionObject->hasContactResponse())
			{
				return 1.0;
			}

			return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
		}


	protected:

		btCollisionObject *mMe;
	};


	CharacterController::CharacterController(LevelObject &charObject, float radius, float height)
	: mCharObject(charObject)
	, mPhysicsManager(charObject.getLevel().getPhysicsManager())
	, mCharacterState(CharacterState::Ok)
	, mStepHeight(0.07)
	, mUp(0, 1, 0)
	, mGravity(0, -0.1, 0)
	{
		mCharShape.reset(new btCapsuleShape(radius, height));
		mGhostObject.reset(new btPairCachingGhostObject());
		mGhostObject->setCollisionShape(mCharShape.get());
		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_CHARACTER_OBJECT);
		mGhostObject->setWorldTransform(BulletAdapter::makeBulletTransform(charObject.getPosition() + osg::Vec3(0, 10, 0), charObject.getRotation()));
		mPhysicsManager.mDynamicsWorld->addCollisionObject(mGhostObject.get(), CollisionGroups::OBJECT, CollisionGroups::ALL);

		mRelativeLowPoint = mUp * -(radius+height*0.5);
	}

	void CharacterController::update(double dt)
	{
		mCurrentPosition = BulletAdapter::toBullet(mCharObject.getPosition());
		mGhostObject->setWorldTransform(btTransform(btQuaternion(0,0,0,1), mCurrentPosition));

		//mCurrentPosition += mVelocity;

		_stepUp();
		bool falling = _stepDown();
		if(falling)
		{
			if(mVelocity.length() < 1)
			{
				mVelocity += mGravity * dt;
			}

		}else
		{
			mVelocity.setZero();
		}

		bool penetration = _hasInvalidPenetrations();
		mCharacterState = penetration ? CharacterState::Penetrated_Object : CharacterState::Ok;

		mCharObject.setPosition(BulletAdapter::toOsg(mCurrentPosition));
	}

	bool CharacterController::_stepUp()
	{
		btVector3 prevPos = mCurrentPosition;

		mCurrentPosition += mUp*mStepHeight;
		mGhostObject->setWorldTransform(btTransform(btQuaternion(0,0,0,1), mCurrentPosition));

		if(_hasInvalidPenetrations())
		{
			// undo up-stepping
			mCurrentPosition = prevPos;
			mGhostObject->setWorldTransform(btTransform(btQuaternion(0,0,0,1), mCurrentPosition));
			return false;

		}else
		{
			return true;
		}
	}

	bool CharacterController::_stepDown()
	{
		btVector3 rayStart = mCurrentPosition + mRelativeLowPoint;
		btVector3 rayEnd = rayStart - mUp*mStepHeight;
		ClosestNotMeRayResultCallback callback(rayStart, rayEnd, mGhostObject.get());
		mPhysicsManager.mDynamicsWorld->rayTest(rayStart, rayEnd, callback);
		if(callback.hasHit() && callback.m_collisionObject != nullptr && _needsCollision(callback.m_collisionObject, mGhostObject.get()))
		{
			mCurrentPosition -= mUp*callback.m_closestHitFraction*mStepHeight;
			return false;

		}else
		{
			// falling
			mCurrentPosition -= mUp*mStepHeight;
			return true;
		}
	}

	bool CharacterController::_hasInvalidPenetrations()
	{
		btVector3 minAabb;
		btVector3 maxAabb;
		mCharShape->getAabb(mGhostObject->getWorldTransform(), minAabb, maxAabb);
		mPhysicsManager.mDynamicsWorld->getBroadphase()->setAabb(mGhostObject->getBroadphaseHandle(), minAabb, maxAabb, mPhysicsManager.mDynamicsWorld->getDispatcher());
		mPhysicsManager.mDynamicsWorld->getDispatcher()->dispatchAllCollisionPairs(mGhostObject->getOverlappingPairCache(), mPhysicsManager.mDynamicsWorld->getDispatchInfo(), mPhysicsManager.mDynamicsWorld->getDispatcher());

		btBroadphasePairArray &pairArray = mGhostObject->getOverlappingPairCache()->getOverlappingPairArray();

		for(size_t pairIndex = 0; pairIndex < pairArray.size(); ++pairIndex)
		{
			btBroadphasePair *collisionPair = &pairArray[pairIndex];

			btCollisionObject *obj0 = static_cast<btCollisionObject*>(collisionPair->m_pProxy0->m_clientObject);
			btCollisionObject *obj1 = static_cast<btCollisionObject*>(collisionPair->m_pProxy1->m_clientObject);

			if((obj0 && !obj0->hasContactResponse()) || (obj1 && !obj1->hasContactResponse()))
			{
				continue;
			}

			if(!_needsCollision(obj0, obj1))
			{
				continue;
			}

			if(collisionPair->m_algorithm)
			{
				collisionPair->m_algorithm->getAllContactManifolds(mManifoldArray);

				for(size_t manifoldIndex = 0; manifoldIndex < mManifoldArray.size(); ++manifoldIndex)
				{
					btPersistentManifold* manifold = mManifoldArray[manifoldIndex];

					btScalar directionSign = (manifold->getBody0() == mGhostObject.get() ? btScalar(-1.0) : btScalar(1.0));
					for(size_t pointIndex = 0; pointIndex < manifold->getNumContacts(); ++pointIndex)
					{
						const btManifoldPoint &pt = manifold->getContactPoint(pointIndex);

						btScalar dist = pt.getDistance();
						if(dist < -0.1/*m_maxPenetrationDepth*/)
						{
							return true;

						}else
						{
							// just touched
						}
					}

				}
			}
		}

		return false;
	}

	bool CharacterController::_needsCollision(const btCollisionObject *body0, const btCollisionObject *body1)
	{
		bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
		collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);
		return collides;
	}





}


