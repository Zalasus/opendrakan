/*
 * Detector.cpp
 *
 *  Created on: Aug 28, 2018
 *      Author: zal
 */

#include <dragonRfl/Detector.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>

namespace od
{

    Detector::Detector(DragonRfl &rfl)
    : mTask(6) // trigger only
    , mDetectWhich(2) // both
    , mDetectMethod(0) // outside to inside
    , mOneWay(false)
    , mTriggerOnlyIfCarryingItem(od::AssetRef::NULL_REF)
    , mInitialState(0) // enabled
    , mTriggerMessage(RflMessage::Off)
    , mDetectOnlyOnce(false)
    , mSequenceToPlay(od::AssetRef::NULL_REF)
    , mMessageString("")
    , mDoesCaveEntranceTeleport(true)
    , mDragonTakesOffUponTeleport(true)
    {
    }

    void Detector::probeFields(FieldProbe &probe)
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

    void Detector::onLoaded(LevelObject &obj)
    {
        obj.setSpawnStrategy(SpawnStrategy::Always);
        obj.setObjectType(LevelObjectType::Detector);
    }

    void Detector::onSpawned(LevelObject &obj)
    {
    }


    OD_REGISTER_RFLCLASS(DragonRfl, Detector);

}



