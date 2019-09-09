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

    class DragonRfl;

    enum class TimerTriggerMode
    {
        DecrementOnAnyMessage,
        DependsOnMessage
    };

    class CountDownToZero : public odRfl::LevelObjectClassBase
    {
    public:

        CountDownToZero();

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    protected:

        odRfl::Integer     mInitialCounterValue;
        odRfl::EnumImpl<TimerTriggerMode, 0, 1> mWhenTriggered;
        odRfl::EnumMessage mMessageToSend;
        odRfl::EnumMessage mIncrementMessage;
        odRfl::EnumMessage mDecrementMessage;
        odRfl::EnumMessage mResetMessage;


    private:

        uint32_t mCounterValue;

    };

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::CountDownToZero, 0x0007, "System", "Count Down To Zero");

#endif /* INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_ */
