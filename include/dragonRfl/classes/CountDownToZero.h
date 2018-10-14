/*
 * CountDownToZero.h
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_
#define INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>
#include <odCore/rfl/RflMessage.h>

namespace dragonRfl
{

    class DragonRfl;

    enum class TimerTriggerMode
    {
        DecrementOnAnyMessage,
        DependsOnMessage
    };

    class CountDownToZero : public odRfl::RflClass
    {
    public:

        CountDownToZero(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded(od::LevelObject &obj) override;
        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message) override;


    protected:

        odRfl::RflInteger     mInitialCounterValue;
        odRfl::RflEnumImpl<TimerTriggerMode, 0, 1> mWhenTriggered;
        odRfl::RflEnumMessage mMessageToSend;
        odRfl::RflEnumMessage mIncrementMessage;
        odRfl::RflEnumMessage mDecrementMessage;
        odRfl::RflEnumMessage mResetMessage;


    private:

        uint32_t mCounterValue;

    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0007, "System", "Count Down To Zero", dragonRfl::CountDownToZero);

#endif /* INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_ */
