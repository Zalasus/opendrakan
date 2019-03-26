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

#include <odCore/physics/Handles.h>
#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{
    class DragonRfl;

    class Detector : public odRfl::RflClass
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

        Detector(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded(od::LevelObject &obj) override;
        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onUpdate(od::LevelObject &obj, float relTime) override;
        virtual bool onCreatePhysicsHandles(od::LevelObject &obj, odPhysics::PhysicsSystem &ps) override;


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

        DragonRfl &mRfl;
        od::RefPtr<odPhysics::ObjectHandle> mPhysicsHandle;
        odPhysics::ContactTestResultVector mResultCache;
        bool mPlayerWasIn;
    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x003d, "System", "Detector", dragonRfl::Detector);

#endif /* INCLUDE_RFL_DRAGON_DETECTOR_H_ */
