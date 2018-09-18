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

namespace od
{

    class Detector : public RflClass
    {
    public:

        Detector();

        virtual void probeFields(FieldProbe &probe) override;
        virtual void onLoaded(LevelObject &obj) override;
        virtual void onSpawned(LevelObject &obj) override;


    protected:

        RflEnum         mTask;
        RflEnum         mDetectWhich;
        RflEnum         mDetectMethod;
        RflEnumYesNo    mOneWay;
        RflClassRef     mTriggerOnlyIfCarryingItem;
        RflEnum         mInitialState;
        RflEnumMessage  mTriggerMessage;
        RflEnumYesNo    mDetectOnlyOnce;
        RflSequenceRef  mSequenceToPlay;
        RflString       mMessageString;
        RflEnumYesNo    mDoesCaveEntranceTeleport;
        RflEnumYesNo    mDragonTakesOffUponTeleport;
    };

}


#endif /* INCLUDE_RFL_DRAGON_DETECTOR_H_ */
