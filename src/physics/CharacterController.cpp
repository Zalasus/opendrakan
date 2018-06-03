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

namespace od
{


	class ClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
	public:

		ClosestNotMeRayResultCallback(const btVector3 &from, const btVector3 &to, btCollisionObject *me)
		: btCollisionWorld::ClosestRayResultCallback(from, to)
		, mMe(me)
		{
			m_collisionFilterGroup = mMe->getBroadphaseHandle()->m_collisionFilterGroup;
			m_collisionFilterMask = mMe->getBroadphaseHandle()->m_collisionFilterMask;
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
			m_collisionFilterGroup = mMe->getBroadphaseHandle()->m_collisionFilterGroup;
			m_collisionFilterMask = mMe->getBroadphaseHandle()->m_collisionFilterMask;
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
	, mVelocity(0, -1, 0)
	{
		mCharShape.reset(new btCapsuleShape(radius, height));
		mGhostObject.reset(new btPairCachingGhostObject());
		mGhostObject->setCollisionShape(mCharShape.get());
		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_CHARACTER_OBJECT);
		mGhostObject->setWorldTransform(BulletAdapter::makeBulletTransform(charObject.getPosition(), charObject.getRotation()));
		mCurrentPosition = BulletAdapter::toBullet(mCharObject.getPosition());
		mPhysicsManager.mDynamicsWorld->addCollisionObject(mGhostObject.get(), CollisionGroups::OBJECT, CollisionGroups::ALL);

		mRelativeLowPoint = mUp * -(radius+height*0.5);
		mRelativeHighPoint = mUp * (radius+height*0.5);
	}

	void CharacterController::update(double dt)
	{
		btVector3 targetPos = BulletAdapter::toBullet(mCharObject.getPosition());

		// simulate slide, ignoring collisions
		mCurrentPosition = targetPos;

		_step(true); // step up
		bool onGround = _step(false); // step down

		//bool penetration = _hasInvalidPenetrations();
		//mCharacterState = penetration ? CharacterState::Penetrated_Object : CharacterState::Ok;

		mGhostObject->setWorldTransform(btTransform(btQuaternion(0,0,0,1), mCurrentPosition));
		mCharObject.setPosition(BulletAdapter::toOsg(mCurrentPosition));
	}

	bool CharacterController::_step(bool up)
	{
		btVector3 rayStart = mCurrentPosition + (up ? mRelativeHighPoint : mRelativeLowPoint);
		btVector3 rayEnd = rayStart + mUp*(up ? mStepHeight : -mStepHeight);
		ClosestNotMeRayResultCallback callback(rayStart, rayEnd, mGhostObject.get());
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


