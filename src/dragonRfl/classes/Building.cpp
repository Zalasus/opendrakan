/*
 * Building.cpp
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#include <dragonRfl/classes/Building.h>

#include <dragonRfl/RflDragon.h>

#include <odCore/rfl/Rfl.h>

#include <odCore/Level.h>
#include <odCore/Engine.h>
#include <odCore/LevelObject.h>

#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{

	Building::Building(DragonRfl &rfl)
	: mInitialHealth(0)
	, mSnapMode(0)
	, mSoundEffectObject(odDb::AssetRef::NULL_REF)
	, mIsDoorWay(false)
	, mCanArokhLandOn(false)
	, mDoorWayAlong(0)
	, mMessageToSend(odRfl::RflMessage::Off)
	, m2DExplosionObject(odDb::AssetRef::NULL_REF)
	, mExplosionGenerator(odDb::AssetRef::NULL_REF)
	, mSmokeGenerator(odDb::AssetRef::NULL_REF)
	, mRubbleAnimation({})
	, mRubbleAnimRate(6.0)
	, mFlammable(true)
	, mPushOverMode(0)
	, mFallWhenDead(true)
	, mHitGroundSound(odDb::AssetRef::NULL_REF)
	, mSendMessageWhenPushed(true)
	, mMessageToSendWhenPushed(odRfl::RflMessage::Off)
	, mSendMessageAfterPushed(true)
	, mMessageToSendAfterPushed(odRfl::RflMessage::Off)
	{
	}

	void Building::probeFields(odRfl::FieldProbe &probe)
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
        DefaultObjectClass::onSpawned(obj);
	}

    void Building::onDespawned(od::LevelObject &obj)
    {
        DefaultObjectClass::onDespawned(obj);
    }


    OD_REGISTER_RFLCLASS(DragonRfl, Building);

}
