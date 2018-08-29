/*
 * CountDownToZero.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#include "rfl/dragon/CountDownToZero.h"

#include <limits>

#include "rfl/Rfl.h"
#include "LevelObject.h"

namespace odRfl
{

    CountDownToZero::CountDownToZero()
    : mInitialCounterValue(1)
    , mWhenTriggered(TimerTriggerMode::DependsOnMessage)
    , mMessageToSend(RflMessage::On)
    , mIncrementMessage(RflMessage::Off)
    , mDecrementMessage(RflMessage::On)
    , mResetMessage(RflMessage::Triggered)
    , mCounterValue(1)
    {
    }

    void CountDownToZero::probeFields(RflFieldProbe &probe)
    {
        probe("Count Down To Zero")
               (mInitialCounterValue, "Initial Counter Value")
               (mWhenTriggered, "When Triggered?")
               (mMessageToSend, "Message To Send")
               (mIncrementMessage, "Increment (+1) Message")
               (mDecrementMessage, "Decrement (-1) Message")
               (mResetMessage, "Reset Message");
    }

    void CountDownToZero::onLoaded(od::LevelObject &obj)
    {
        obj.setSpawnStrategy(od::SpawnStrategy::Always);
    }

    void CountDownToZero::onSpawned(od::LevelObject &obj)
    {
        mCounterValue = mInitialCounterValue;
    }

    void CountDownToZero::onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message)
    {
        uint32_t newCounter = mCounterValue;

        switch(mWhenTriggered)
        {
        case TimerTriggerMode::DecrementOnAnyMessage:
            if(newCounter > 0)
            {
                --newCounter;
            }
            break;

        case TimerTriggerMode::DependsOnMessage:
            if(message == mIncrementMessage)
            {
                if(newCounter < std::numeric_limits<uint32_t>::max())
                {
                    ++mCounterValue;
                }

            }else if(message == mDecrementMessage)
            {
                if(newCounter > 0)
                {
                    --newCounter;
                }

            }else if(message == mResetMessage)
            {
                newCounter = mInitialCounterValue;
            }
            break;

        default:
            break;
        }

        // only trigger when we hit 0, not when we are already at zero when receiving message TODO: is this the right behaviour?
        if(mCounterValue != 0 && newCounter == 0)
        {
            obj.messageAllLinkedObjects(mMessageToSend);
        }

        Logger::verbose() << "Counter " << obj.getObjectId() << " received message '" << message << "' from " << sender.getObjectId() << ". "
                << "Count was " << mCounterValue << ", now is " << newCounter;

        mCounterValue = newCounter;
    }


    OD_REGISTER_RFL_CLASS(0x0007, "Count Down To Zero", CountDownToZero);

}



