/*
 * Building.cpp
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#include "rfl/dragon/Building.h"

#include "rfl/Rfl.h"
#include "Level.h"
#include "Engine.h"
#include "physics/PhysicsManager.h"
#include "LevelObject.h"

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

    void Building::spawned(od::LevelObject &obj)
	{
    	if(obj.getModel() != nullptr && obj.getModel()->getModelBounds() != nullptr)
		{
    		obj.getLevel().getPhysicsManager().addObject(obj, 0);
    	}
	}

    void Building::despawned(od::LevelObject &obj)
    {
    	obj.getLevel().getPhysicsManager().removeObject(obj);
    }

    OD_REGISTER_RFL_CLASS(0x0011, "Building", Building);

}
