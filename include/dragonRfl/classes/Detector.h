/*
 * Detector.h
 *
 *  Created on: Aug 28, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_DETECTOR_H_
#define INCLUDE_RFL_DRAGON_DETECTOR_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

#include <odCore/physics/Handles.h>
#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{
    class DragonRfl;

    class Detector : public odRfl::LevelObjectClassBase
    {
    public:

        enum class Task
        {
            TransferLink,
            EnableDisableSaving,
            MessageScreen,
            NoFlyZone,
            PlaySequence,
            Reserved,
            TriggerOnly,
            CaveEntrance,
            Message,
            TeleportDragon
        };
        typedef odRfl::EnumImpl<Task, 0, 9> EnumTask;

        enum class DetectWhich
        {
            RynnOnGround,
            RynnOnDragon,
            Both, // both of the above! not rynn or dragon!
            RynnOrDragonOrNpcs
        };
        typedef odRfl::EnumImpl<DetectWhich, 0, 3> EnumDetectWhich;

        enum class DetectMethod
        {
            OutsideToInside,
            InsideToOutside
        };
        typedef odRfl::EnumImpl<DetectMethod, 0, 1> EnumDetectMethod;

        enum class InitialState
        {
            Enabled, // yeah, they honestly thought 0 would be good for indicating enabled
            Disabled
        };
        typedef odRfl::EnumImpl<InitialState, 0, 1> EnumInitialState;

        Detector();

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onUpdate(float relTime) override;


    protected:

        EnumTask            mTask;
        EnumDetectWhich     mDetectWhich;
        EnumDetectMethod    mDetectMethod;
        odRfl::EnumYesNo    mOneWay;
        odRfl::ClassRef     mTriggerOnlyIfCarryingItem;
        EnumInitialState    mInitialState;
        odRfl::EnumMessage  mTriggerMessage;
        odRfl::EnumYesNo    mDetectOnlyOnce;
        odRfl::SequenceRef  mSequenceToPlay;
        odRfl::String       mMessageString;
        odRfl::EnumYesNo    mDoesCaveEntranceTeleport;
        odRfl::EnumYesNo    mDragonTakesOffUponTeleport;

    private:

        od::RefPtr<odPhysics::ObjectHandle> mPhysicsHandle;
        odPhysics::ContactTestResultVector mResultCache;
        bool mPlayerWasIn;
    };

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::Detector, 0x003d, "System", "Detector");

#endif /* INCLUDE_RFL_DRAGON_DETECTOR_H_ */
