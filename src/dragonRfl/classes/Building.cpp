/*
 * Building.cpp
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#include <dragonRfl/classes/Building.h>

#include <dragonRfl/RflDragon.h>

#include <odCore/rfl/Rfl.h>

#include <odCore/Client.h>
#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Layer.h>

#include <odCore/physics/PhysicsSystem.h>

#include <odCore/render/Renderer.h>

namespace dragonRfl
{

    BuildingFields::BuildingFields()
    : initialHealth(0)
	, snapMode(0)
	, soundEffectObject(odDb::AssetRef::NULL_REF)
	, isDoorWay(false)
	, canArokhLandOn(false)
	, doorWayAlong(0)
	, messageToSend(od::Message::Off)
	, m2DExplosionObject(odDb::AssetRef::NULL_REF)
	, explosionGenerator(odDb::AssetRef::NULL_REF)
	, smokeGenerator(odDb::AssetRef::NULL_REF)
	, rubbleAnimation({})
	, rubbleAnimRate(6.0)
	, flammable(true)
	, pushOverMode(0)
	, fallWhenDead(true)
	, hitGroundSound(odDb::AssetRef::NULL_REF)
	, sendMessageWhenPushed(true)
	, messageToSendWhenPushed(od::Message::Off)
	, sendMessageAfterPushed(true)
	, messageToSendAfterPushed(od::Message::Off)
    {
    }

    void BuildingFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Basics")
				(initialHealth, "Initial Health")
				(snapMode, "Snap Mode")
				(soundEffectObject, "Sound Effect Object")
				(isDoorWay, "Is Door Way?")
				(canArokhLandOn, "Can Arokh Land On?")
				(doorWayAlong, "Door Way Along")
			("Destruction")
				(messageToSend, "Message To Send")
				(m2DExplosionObject, "2D Explosion Object")
				(explosionGenerator, "Explosion Generator")
				(smokeGenerator, "Smoke Generator")
				(rubbleAnimation, "Rubble Animation")
				(rubbleAnimRate, "Rubble Anim Rate")
				(flammable, "Flammable")
			("Physics")
				(pushOverMode, "Push Over Mode")
				(fallWhenDead, "Fall When Dead")
				(hitGroundSound, "Hit Ground Sound")
				(sendMessageWhenPushed, "Send Message When Pushed?")
				(messageToSendWhenPushed, "Message To Send When Pushed")
				(sendMessageAfterPushed, "Send Message After Pushed?")
				(messageToSendAfterPushed, "Message to Send After Pushed");
    }


    void Building_Sv::onSpawned()
	{
	}

    void Building_Sv::onDespawned()
    {
    }


    Building_Cl::Building_Cl()
    : mHealth(0.0)
    {
    }

    void Building_Cl::onLoaded()
    {
        mHealth = mFields.initialHealth;
    }

    void Building_Cl::onSpawned()
    {
        auto &obj = getLevelObject();

        obj.setupRenderingAndPhysics(od::ObjectRenderMode::NORMAL, od::ObjectPhysicsMode::SOLID);
    }

    DamageResult Building_Cl::onAttackHit(od::LevelObject &attacker, const Damage &damage)
    {
        mHealth -= damage.strength;
        if(mHealth < 0) mHealth = 0;

        return DamageResult::HIT;
    }

}
