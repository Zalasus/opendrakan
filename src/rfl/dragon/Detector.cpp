/*
 * Detector.cpp
 *
 *  Created on: Aug 28, 2018
 *      Author: zal
 */

#include "rfl/dragon/Detector.h"

#include "rfl/Rfl.h"

namespace odRfl
{

    Detector::Detector()
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

    void Detector::probeFields(RflFieldProbe &probe)
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

    void Detector::spawned(od::LevelObject &obj)
    {

    }


    OD_REGISTER_RFL_CLASS(0x003d, "Detector", Detector);

}



