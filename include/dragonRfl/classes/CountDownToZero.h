/*
 * CountDownToZero.h
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_
#define INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/DummyClass.h>

namespace dragonRfl
{

    enum class TimerTriggerMode
    {
        DecrementOnAnyMessage,
        DependsOnMessage
    };


    struct CountDownToZeroFields final : public odRfl::FieldBundle
    {
        CountDownToZeroFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::Integer                          initialCounterValue;
        odRfl::EnumImpl<TimerTriggerMode, 0, 1> whenTriggered;
        odRfl::EnumMessage                      messageToSend;
        odRfl::EnumMessage                      incrementMessage;
        odRfl::EnumMessage                      decrementMessage;
        odRfl::EnumMessage                      resetMessage;
    };


    /*struct CountDownToZeroStates final : public odState::StateBundle
    {
        virtual void probeStates(odState::StateProbe &probe) override
        {
            probe(counterValue);
        }

        odState::State<uint32_t, odState::SAVED> counterValue;
    };*/


    class CountDownToZero_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<CountDownToZero_Sv>
    {
    public:

        CountDownToZero_Sv();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }
        //virtual odState::StateBundle *getStates() override { return &mStates; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    private:

        CountDownToZeroFields mFields;
        int mCounterValue;
        //CountDownToZeroStates mStates;

    };


    using CountDownToZeroFactory = odRfl::DefaultClassFactory<CountDownToZeroFields, odRfl::DummyClass, CountDownToZero_Sv>;

    OD_DEFINE_CLASS(CountDownToZero, 0x0007, "System", "Count Down To Zero", CountDownToZeroFactory);

}

#endif /* INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_ */
