/*
 * Detector.cpp
 *
 *  Created on: Aug 28, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/Detector.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>

namespace dragonRfl
{

    Detector::Detector(DragonRfl &rfl)
    : mTask(6) // trigger only
    , mDetectWhich(2) // both
    , mDetectMethod(0) // outside to inside
    , mOneWay(false)
    , mTriggerOnlyIfCarryingItem(odDb::AssetRef::NULL_REF)
    , mInitialState(0) // enabled
    , mTriggerMessage(odRfl::RflMessage::Off)
    , mDetectOnlyOnce(false)
    , mSequenceToPlay(odDb::AssetRef::NULL_REF)
    , mMessageString("")
    , mDoesCaveEntranceTeleport(true)
    , mDragonTakesOffUponTeleport(true)
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
        obj.setSpawnStrategy(od::SpawnStrategy::Always);
        obj.setObjectType(od::LevelObjectType::Detector);
    }

    void Detector::onSpawned(od::LevelObject &obj)
    {
    }


    OD_REGISTER_RFLCLASS(DragonRfl, Detector);

}



