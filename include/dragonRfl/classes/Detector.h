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
#include <odCore/rfl/DummyClass.h>

#include <odCore/physics/Handles.h>
#include <odCore/physics/PhysicsSystem.h>

namespace dragonRfl
{

    struct DetectorFields final : public odRfl::FieldBundle
    {

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

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        EnumTask            task;
        EnumDetectWhich     detectWhich;
        EnumDetectMethod    detectMethod;
        odRfl::EnumYesNo    oneWay;
        odRfl::ClassRef     triggerOnlyIfCarryingItem;
        EnumInitialState    initialState;
        odRfl::EnumMessage  triggerMessage;
        odRfl::EnumYesNo    detectOnlyOnce;
        odRfl::SequenceRef  sequenceToPlay;
        odRfl::String       messageString;
        odRfl::EnumYesNo    doesCaveEntranceTeleport;
        odRfl::EnumYesNo    dragonTakesOffUponTeleport;
    };


    class Detector_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<Detector_Sv>
    {
    public:

        Detector_Sv();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onUpdate(float relTime) override;


    private:

        DetectorFields mFields;

        std::shared_ptr<odPhysics::ObjectHandle> mPhysicsHandle;
        odPhysics::ContactTestResultVector mResultCache;
        bool mPlayerWasIn;
    };


    using DetectorFactory = odRfl::ServerOnlyClassFactory<DetectorFields, Detector_Sv>;


    OD_DEFINE_CLASS(Detector, 0x003d, "System", "Detector", DetectorFactory);

}

#endif /* INCLUDE_RFL_DRAGON_DETECTOR_H_ */
