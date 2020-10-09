/*
 * Detector.cpp
 *
 *  Created on: Aug 28, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/Detector.h>

#include <dragonRfl/RflDragon.h>
#include <dragonRfl/classes/HumanControl.h>

#include <odCore/Server.h>
#include <odCore/LevelObject.h>

#include <odCore/rfl/Rfl.h>

namespace dragonRfl
{

    DetectorFields::DetectorFields()
    : task(Task::TRIGGER_ONLY)
    , detectWhich(DetectWhich::BOTH)
    , detectMethod(DetectMethod::OUTSIDE_TO_INSIDE)
    , oneWay(false)
    , triggerOnlyIfCarryingItem(odDb::AssetRef::NULL_REF)
    , initialState(InitialState::ENABLED)
    , triggerMessage(od::Message::Off)
    , detectOnlyOnce(false)
    , sequenceToPlay(odDb::AssetRef::NULL_REF)
    , messageString("")
    , doesCaveEntranceTeleport(true)
    , dragonTakesOffUponTeleport(true)
    {
    }

    void DetectorFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Detector")
                (task, "Task")
                (detectWhich, "Detect Which?")
                (detectMethod, "Detect Method")
                (oneWay, "One Way?")
                (triggerOnlyIfCarryingItem, "Trigger Only If Carrying Item...")
                (initialState, "Initial State")
                (triggerMessage, "Trigger Message")
                (detectOnlyOnce, "Detect Only Once?")
                (sequenceToPlay, "Sequence To Play")
                (messageString, "Message String")
                (doesCaveEntranceTeleport, "Does Cave Entrance Teleport?")
                (dragonTakesOffUponTeleport, "Dragon Takes Off Upon Teleport?");
    }


    Detector_Sv::Detector_Sv()
    : mPlayerWasIn(false)
    {
    }

    void Detector_Sv::onLoaded()
    {
    }

    void Detector_Sv::onSpawned()
    {
        getLevelObject().setEnableUpdate(true);

        mPhysicsHandle = getServer().getPhysicsSystem().createObjectHandle(getLevelObject(), true);
        mPhysicsHandle->setEnableCollision(false);
    }

    void Detector_Sv::onUpdate(float relTime)
    {
        if(mFields.task != DetectorFields::Task::TRIGGER_ONLY || mPhysicsHandle == nullptr)
        {
            return;
        }

        od::LevelObject &obj = getLevelObject();

        // TODO: since we currently only can detect the player (no definition of "NPC" exists yet), we could speed things up a bit by only
        //  performing the costly contact test if the player position is inside our bounding sphere. later we should further improve this
        //  by using a "single contact test" for the methods that only detect the player.

        mResultCache.clear();
        getServer().getPhysicsSystem().contactTest(mPhysicsHandle, odPhysics::PhysicsTypeMasks::LevelObject, mResultCache);

        bool playerIsIn = false;
        for(auto &result : mResultCache)
        {
            odPhysics::ObjectHandle *objectHandle = result.handle->asObjectHandle();
            if(objectHandle == nullptr)
            {
                continue;
            }

            // the server has no notion on what a "player" is, yet. to not break the detector, we check whether the
            //  object in question is of the HumanControl class
            odRfl::ClassId objectClassId = objectHandle->getLevelObject().getClass()->getRflClassId();
            if(objectClassId == HumanControl::classId())
            {
                playerIsIn = true;
                break;
            }
        }

        bool triggered = false;
        switch(mFields.detectMethod)
        {
        case DetectorFields::DetectMethod::INSIDE_TO_OUTSIDE:
            triggered = mPlayerWasIn && !playerIsIn;
            break;

        case DetectorFields::DetectMethod::OUTSIDE_TO_INSIDE:
            triggered = !mPlayerWasIn && playerIsIn;
            break;
        }

        if(triggered)
        {
            obj.messageAllLinkedObjects(mFields.triggerMessage);
        }

        mPlayerWasIn = playerIsIn;
    }

}
