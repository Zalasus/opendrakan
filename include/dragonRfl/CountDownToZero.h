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

namespace od
{

    class DragonRfl;

    enum class TimerTriggerMode
    {
        DecrementOnAnyMessage,
        DependsOnMessage
    };

    class CountDownToZero : public RflClass
    {
    public:

        CountDownToZero(DragonRfl &rfl);

        virtual void probeFields(FieldProbe &probe) override;
        virtual void onLoaded(LevelObject &obj) override;
        virtual void onSpawned(LevelObject &obj) override;
        virtual void onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message) override;


    protected:

        RflInteger     mInitialCounterValue;
        RflEnumImpl<TimerTriggerMode, 0, 1> mWhenTriggered;
        RflEnumMessage mMessageToSend;
        RflEnumMessage mIncrementMessage;
        RflEnumMessage mDecrementMessage;
        RflEnumMessage mResetMessage;


    private:

        uint32_t mCounterValue;

    };


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x0007, "System", "Count Down To Zero", CountDownToZero);

}


#endif /* INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_ */
