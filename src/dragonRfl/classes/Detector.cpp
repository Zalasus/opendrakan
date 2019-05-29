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

    Detector::Detector(DragonRfl &rfl)
    : mTask(Task::TriggerOnly)
    , mDetectWhich(DetectWhich::Both)
    , mDetectMethod(DetectMethod::OutsideToInside)
    , mOneWay(false)
    , mTriggerOnlyIfCarryingItem(odDb::AssetRef::NULL_REF)
    , mInitialState(InitialState::Enabled)
    , mTriggerMessage(odRfl::RflMessage::Off)
    , mDetectOnlyOnce(false)
    , mSequenceToPlay(odDb::AssetRef::NULL_REF)
    , mMessageString("")
    , mDoesCaveEntranceTeleport(true)
    , mDragonTakesOffUponTeleport(true)
    , mRfl(rfl)
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

    void Detector::onLoaded(od::LevelObject &obj)
    {
        obj.setObjectType(od::LevelObjectType::Detector);
    }

    void Detector::onSpawned(od::LevelObject &obj)
    {
        obj.setEnableRflUpdateHook(true);

        mPhysicsHandle = obj.getLevel().getEngine().getPhysicsSystem().createObjectHandle(obj, true);
        mPhysicsHandle->setEnableCollision(false);
    }

    void Detector::onUpdate(od::LevelObject &obj, float relTime)
    {
        if(mTask != Task::TriggerOnly || mPhysicsHandle == nullptr)
        {
            return;
        }

        od::LevelObject *playerObject = (mRfl.getLocalPlayer() != nullptr) ? &mRfl.getLocalPlayer()->getLevelObject() : nullptr;

        mResultCache.clear();
        obj.getLevel().getEngine().getPhysicsSystem().contactTest(mPhysicsHandle, odPhysics::PhysicsTypeMasks::All, mResultCache);

        bool playerIsIn = false;
        for(auto &result : mResultCache)
        {
            odPhysics::ObjectHandle *objectHandle = result.handle->asObjectHandle();
            if(objectHandle == nullptr)
            {
                continue;
            }

            if(&objectHandle->getLevelObject() == playerObject)
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

    OD_REGISTER_RFLCLASS(DragonRfl, Detector);

}



