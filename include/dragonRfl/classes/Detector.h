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

    class DetectorFields : public odRfl::FieldBundle
    {
    public:

        enum class Task
        {
            TRANSFER_LINK,
            ENABLE_DISABLE_SAVING,
            MESSAGE_SCREEN,
            NO_FLY_ZONE,
            PLAY_SEQUENCE,
            RESERVED,
            TRIGGER_ONLY,
            CAVE_ENTRANCE,
            MESSAGE,
            TELEPORT_DRAGON
        };
        typedef odRfl::EnumImpl<Task, 0, 9> EnumTask;

        enum class DetectWhich
        {
            RYNN_ON_GROUND,
            RYNN_ON_DRAGON,
            BOTH, // both of the above! not rynn or dragon!
            RYNN_ON_DRAGON_OR_NPCS
        };
        typedef odRfl::EnumImpl<DetectWhich, 0, 3> EnumDetectWhich;

        enum class DetectMethod
        {
            OUTSIDE_TO_INSIDE,
            INSIDE_TO_OUTSIDE
        };
        typedef odRfl::EnumImpl<DetectMethod, 0, 1> EnumDetectMethod;

        enum class InitialState
        {
            ENABLED, // yeah, they honestly thought 0 would be good for indicating enabled
            DISABLED
        };
        typedef odRfl::EnumImpl<InitialState, 0, 1> EnumInitialState;

        DetectorFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override final;


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

    };


    class Detector : public odRfl::ClassBase, private odRfl::Spawnable, private odRfl::ServerClassImpl, private DetectorFields
    {
    public:

        Detector(od::Server &server);

        virtual odRfl::Spawnable *getSpawnable() override final { return this; }
        virtual odRfl::FieldBundle *getFieldBundle() override final { return this; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onUpdate(float relTime) override;


    private:

        od::RefPtr<odPhysics::ObjectHandle> mPhysicsHandle;
        odPhysics::ContactTestResultVector mResultCache;
        bool mPlayerWasIn;
    };

}

ODRFL_DEFINE_CLASS(dragonRfl::Detector, 0x003d, "System", "Detector");
ODRFL_DEFINE_CLASS_FIELDS(dragonRfl::Detector, dragonRfl::DetectorFields);
ODRFL_DEFINE_CLASS_IMPL_SERVER(dragonRfl::Detector, dragonRfl::Detector);

#endif /* INCLUDE_RFL_DRAGON_DETECTOR_H_ */
