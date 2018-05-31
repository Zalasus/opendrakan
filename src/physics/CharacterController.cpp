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

	CharacterController::CharacterController(LevelObject &charObject, float radius, float height)
	: mCharObject(charObject)
	, mPhysicsManager(charObject.getLevel().getPhysicsManager())
	{
		mCharShape.reset(new btCapsuleShape(radius, height));
		mGhostObject.reset(new btPairCachingGhostObject());
		mGhostObject->setCollisionShape(mCharShape.get());
		mGhostObject->setWorldTransform(BulletAdapter::makeBulletTransform(charObject.getPosition() + osg::Vec3(0, 10, 0), charObject.getRotation()));
		mPhysicsManager.mDynamicsWorld->addCollisionObject(mGhostObject.get(), CollisionGroups::OBJECT, CollisionGroups::ALL);

		mBtController.reset(new btKinematicCharacterController(mGhostObject.get(), mCharShape.get(), 0.05));
		mPhysicsManager.mDynamicsWorld->addAction(mBtController.get());
	}

	void CharacterController::update()
	{
		btTransform xf = mGhostObject->getWorldTransform();
		mCharObject.setPosition(BulletAdapter::toOsg(xf.getOrigin()));
	}

}
