/*
 * CountDownToZero.h
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_
#define INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"
#include "rfl/RflMessage.h"

namespace odRfl
{

    enum class TimerTriggerMode
    {
        DecrementOnAnyMessage,
        DependsOnMessage
    };

    class CountDownToZero : public RflClass
    {
    public:

        CountDownToZero();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void loaded(od::Engine &engine, od::LevelObject *obj) override;
        virtual void spawned(od::LevelObject &obj) override;
        virtual void messageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message) override;


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

}


#endif /* INCLUDE_RFL_DRAGON_COUNTDOWNTOZERO_H_ */
