/*
 * Detector.h
 *
 *  Created on: Aug 28, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_DETECTOR_H_
#define INCLUDE_RFL_DRAGON_DETECTOR_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace dragonRfl
{

    class DragonRfl;

    class Detector : public odRfl::RflClass
    {
    public:

        Detector(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded(od::LevelObject &obj) override;
        virtual void onSpawned(od::LevelObject &obj) override;


    protected:

        odRfl::RflEnum         mTask;
        odRfl::RflEnum         mDetectWhich;
        odRfl::RflEnum         mDetectMethod;
        odRfl::RflEnumYesNo    mOneWay;
        odRfl::RflClassRef     mTriggerOnlyIfCarryingItem;
        odRfl::RflEnum         mInitialState;
        odRfl::RflEnumMessage  mTriggerMessage;
        odRfl::RflEnumYesNo    mDetectOnlyOnce;
        odRfl::RflSequenceRef  mSequenceToPlay;
        odRfl::RflString       mMessageString;
        odRfl::RflEnumYesNo    mDoesCaveEntranceTeleport;
        odRfl::RflEnumYesNo    mDragonTakesOffUponTeleport;
    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x003d, "System", "Detector", dragonRfl::Detector);

#endif /* INCLUDE_RFL_DRAGON_DETECTOR_H_ */
