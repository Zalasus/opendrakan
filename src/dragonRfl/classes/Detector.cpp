/*
 * Detector.cpp
 *
 *  Created on: Aug 28, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/Detector.h>

#include <dragonRfl/RflDragon.h>
#include <dragonRfl/LocalPlayer.h>

#include <odCore/rfl/Rfl.h>

#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/Engine.h>

namespace dragonRfl
{

    Detector::Detector()
    : mTask(Task::TriggerOnly)
    , mDetectWhich(DetectWhich::Both)
    , mDetectMethod(DetectMethod::OutsideToInside)
    , mOneWay(false)
    , mTriggerOnlyIfCarryingItem(odDb::AssetRef::NULL_REF)
    , mInitialState(InitialState::Enabled)
    , mTriggerMessage(od::Message::Off)
    , mDetectOnlyOnce(false)
    , mSequenceToPlay(odDb::AssetRef::NULL_REF)
    , mMessageString("")
    , mDoesCaveEntranceTeleport(true)
    , mDragonTakesOffUponTeleport(true)
    , mPlayerWasIn(false)
    {
    }

    void Detector::probeFields(odRfl::FieldProbe &probe)
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

    void Detector::onLoaded()
    {
        getLevelObject().setObjectType(od::LevelObjectType::Detector);
    }

    void Detector::onSpawned()
    {
        getLevelObject().setEnableRflUpdateHook(true);

        mPhysicsHandle = getLevelObject().getLevel().getEngine().getPhysicsSystem().createObjectHandle(getLevelObject(), true);
        mPhysicsHandle->setEnableCollision(false);
    }

    void Detector::onUpdate(float relTime)
    {
        if(mTask != Task::TriggerOnly || mPhysicsHandle == nullptr)
        {
            return;
        }

        if(mRfl.getLocalPlayer() != nullptr)
        {
            return;
        }

        od::LevelObject &obj = getLevelObject();
        od::LevelObject &playerObject = mRfl.getLocalPlayer()->getLevelObject();

        // since we currently only can detect the player (no definition of "NPC" exists yet), we can speed things up a bit by only performing
        // the costly contact test if the player position is inside our bounding sphere. later we should further improve this by using a
        // "single contact test" for the methods that only detect the player. the physics system does not have that interface yet, though.
        if(!obj.getBoundingSphere().contains(playerObject.getPosition()))
        {
            return;
        }

        mResultCache.clear();
        obj.getLevel().getEngine().getPhysicsSystem().contactTest(mPhysicsHandle, odPhysics::PhysicsTypeMasks::LevelObject, mResultCache);

        bool playerIsIn = false;
        for(auto &result : mResultCache)
        {
            odPhysics::ObjectHandle *objectHandle = result.handle->asObjectHandle();
            if(objectHandle == nullptr)
            {
                continue;
            }

            if(&objectHandle->getLevelObject() == &playerObject)
            {
                playerIsIn = true;
                break;
            }
        }

        bool triggered = false;
        switch(mDetectMethod)
        {
        case DetectMethod::InsideToOutside:
            triggered = mPlayerWasIn && !playerIsIn;
            break;

        case DetectMethod::OutsideToInside:
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



