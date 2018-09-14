/*
 * Building.cpp
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#include <dragonRfl/Building.h>

#include <odCore/rfl/Rfl.h>
#include <odCore/Level.h>
#include <odCore/Engine.h>
#include <odCore/physics/PhysicsManager.h>
#include <odCore/LevelObject.h>

namespace odRfl
{

	Building::Building()
	: mInitialHealth(0)
	, mSnapMode(0)
	, mSoundEffectObject(od::AssetRef::NULL_REF)
	, mIsDoorWay(false)
	, mCanArokhLandOn(false)
	, mDoorWayAlong(0)
	, mMessageToSend(RflMessage::Off)
	, m2DExplosionObject(od::AssetRef::NULL_REF)
	, mExplosionGenerator(od::AssetRef::NULL_REF)
	, mSmokeGenerator(od::AssetRef::NULL_REF)
	, mRubbleAnimation({})
	, mRubbleAnimRate(6.0)
	, mFlammable(true)
	, mPushOverMode(0)
	, mFallWhenDead(true)
	, mHitGroundSound(od::AssetRef::NULL_REF)
	, mSendMessageWhenPushed(true)
	, mMessageToSendWhenPushed(RflMessage::Off)
	, mSendMessageAfterPushed(true)
	, mMessageToSendAfterPushed(RflMessage::Off)
	{
	}

	void Building::probeFields(RflFieldProbe &probe)
    {
		probe("Basics")
				(mInitialHealth, "Initial Health")
				(mSnapMode, "Snap Mode")
				(mSoundEffectObject, "Sound Effect Object")
				(mIsDoorWay, "Is Door Way?")
				(mCanArokhLandOn, "Can Arokh Land On?")
				(mDoorWayAlong, "Door Way Along")
			("Destruction")
				(mMessageToSend, "Message To Send")
				(m2DExplosionObject, "2D Explosion Object")
				(mExplosionGenerator, "Explosion Generator")
				(mSmokeGenerator, "Smoke Generator")
				(mRubbleAnimation, "Rubble Animation")
				(mRubbleAnimRate, "Rubble Anim Rate")
				(mFlammable, "Flammable")
			("Physics")
				(mPushOverMode, "Push Over Mode")
				(mFallWhenDead, "Fall When Dead")
				(mHitGroundSound, "Hit Ground Sound")
				(mSendMessageWhenPushed, "Send Message When Pushed?")
				(mMessageToSendWhenPushed, "Message To Send When Pushed")
				(mSendMessageAfterPushed, "Send Message After Pushed?")
				(mMessageToSendAfterPushed, "Message to Send After Pushed");
    }

    void Building::onSpawned(od::LevelObject &obj)
	{
    	if(obj.getClass()->getModel() != nullptr && obj.getClass()->getModel()->getModelBounds() != nullptr)
		{
    		obj.getLevel().getPhysicsManager().addObject(obj, 0);
    	}
	}

    void Building::onDespawned(od::LevelObject &obj)
    {
    	obj.getLevel().getPhysicsManager().removeObject(obj);
    }

    OD_REGISTER_RFL_CLASS(0x0011, "Building", Building);

}
