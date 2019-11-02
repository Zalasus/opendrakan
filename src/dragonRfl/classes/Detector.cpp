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
    : mTask(Task::TRIGGER_ONLY)
    , mDetectWhich(DetectWhich::BOTH)
    , mDetectMethod(DetectMethod::OUTSIDE_TO_INSIDE)
    , mOneWay(false)
    , mTriggerOnlyIfCarryingItem(odDb::AssetRef::NULL_REF)
    , mInitialState(InitialState::ENABLED)
    , mTriggerMessage(od::Message::Off)
    , mDetectOnlyOnce(false)
    , mSequenceToPlay(odDb::AssetRef::NULL_REF)
    , mMessageString("")
    , mDoesCaveEntranceTeleport(true)
    , mDragonTakesOffUponTeleport(true)
    {
    }

    void DetectorFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Detector")
                (mTask, "Task")
                (mDetectWhich, "Detect Which?")
                (mDetectMethod, "Detect Method")
                (mOneWay, "One Way?")
                (mTriggerOnlyIfCarryingItem, "Trigger Only If Carrying Item...")
                (mInitialState, "Initial State")
                (mTriggerMessage, "Trigger Message")
                (mDetectOnlyOnce, "Detect Only Once?")
                (mSequenceToPlay, "Sequence To Play")
                (mMessageString, "Message String")
                (mDoesCaveEntranceTeleport, "Does Cave Entrance Teleport?")
                (mDragonTakesOffUponTeleport, "Dragon Takes Off Upon Teleport?");
    }


    Detector::Detector(od::Server &server)
    : odRfl::ServerClassImpl(server)
    , mPlayerWasIn(false)
    {
    }

    void Detector::onLoaded()
    {
        getLevelObject().setObjectType(od::LevelObjectType::Detector);
    }

    void Detector::onSpawned()
    {
        getLevelObject().setEnableUpdate(true);

        od::Server &server = getServer();
        mPhysicsHandle = server.getPhysicsSystem().createObjectHandle(getLevelObject(), true);
        mPhysicsHandle->setEnableCollision(false);
    }

    void Detector::onUpdate(float relTime)
    {
        if(mTask != Task::TRIGGER_ONLY || mPhysicsHandle == nullptr)
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
            if(objectClassId == odRfl::ClassTraits<HumanControl>::classId())
            {
                playerIsIn = true;
                break;
            }
        }

        bool triggered = false;
        switch(mDetectMethod)
        {
        case DetectMethod::INSIDE_TO_OUTSIDE:
            triggered = mPlayerWasIn && !playerIsIn;
            break;

        case DetectMethod::OUTSIDE_TO_INSIDE:
            triggered = !mPlayerWasIn && playerIsIn;
            break;
        }

        if(triggered)
        {
            obj.messageAllLinkedObjects(mTriggerMessage);
        }

        mPlayerWasIn = playerIsIn;
    }

}



