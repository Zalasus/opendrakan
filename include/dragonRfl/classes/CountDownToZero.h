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

namespace dragonRfl
{
    enum class TimerTriggerMode
    {
        DecrementOnAnyMessage,
        DependsOnMessage
    };

    class CountDownToZeroFields final : public odRfl::FieldBundle
    {
    public:

        CountDownToZeroFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::Integer                          initialCounterValue;
        odRfl::EnumImpl<TimerTriggerMode, 0, 1> whenTriggered;
        odRfl::EnumMessage                      messageToSend;
        odRfl::EnumMessage                      incrementMessage;
        odRfl::EnumMessage                      decrementMessage;
        odRfl::EnumMessage                      resetMessage;

    };

    class CountDownToZero_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<CountDownToZero_Sv>
    {
    public:

        CountDownToZero_Sv();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    private:

        CountDownToZeroFields mFields;

        uint32_t mCounterValue;

    };

    // This is effectively a dummy class. In theory, we could add a dedicated Dummy class implementation for that purpose
    class CountDownToZero_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<CountDownToZero_Cl>
    {
    public:

        CountDownToZero_Cl() = default;

        virtual odRfl::FieldBundle &getFields() override { return mFields; }


    private:

        CountDownToZeroFields mFields;

    };

    using CountDownToZeroFactory = odRfl::DefaultClassFactory<CountDownToZeroFields, CountDownToZero_Cl, CountDownToZero_Sv>;

    OD_DEFINE_CLASS(CountDownToZero, 0x0007, "System", "Count Down To Zero", CountDownToZeroFactory);

}



#endif /* INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_ */
