/*
 * Detector.h
 *
 *  Created on: Aug 28, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_DETECTOR_H_
#define INCLUDE_RFL_DRAGON_DETECTOR_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

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

        odRfl::Enum         mTask;
        odRfl::Enum         mDetectWhich;
        odRfl::Enum         mDetectMethod;
        odRfl::EnumYesNo    mOneWay;
        odRfl::ClassRef     mTriggerOnlyIfCarryingItem;
        odRfl::Enum         mInitialState;
        odRfl::EnumMessage  mTriggerMessage;
        odRfl::EnumYesNo    mDetectOnlyOnce;
        odRfl::SequenceRef  mSequenceToPlay;
        odRfl::String       mMessageString;
        odRfl::EnumYesNo    mDoesCaveEntranceTeleport;
        odRfl::EnumYesNo    mDragonTakesOffUponTeleport;
    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x003d, "System", "Detector", dragonRfl::Detector);

#endif /* INCLUDE_RFL_DRAGON_DETECTOR_H_ */
