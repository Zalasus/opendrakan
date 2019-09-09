/*
 * CountDownToZero.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/CountDownToZero.h>

#include <limits>

#include <odCore/LevelObject.h>
#include <odCore/rfl/Rfl.h>

#include <dragonRfl/RflDragon.h>

namespace dragonRfl
{

    CountDownToZero::CountDownToZero()
    : mInitialCounterValue(1)
    , mWhenTriggered(TimerTriggerMode::DependsOnMessage)
    , mMessageToSend(od::Message::On)
    , mIncrementMessage(od::Message::Off)
    , mDecrementMessage(od::Message::On)
    , mResetMessage(od::Message::Triggered)
    , mCounterValue(1)
    {
    }

    void CountDownToZero::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Count Down To Zero")
               (mInitialCounterValue, "Initial Counter Value")
               (mWhenTriggered, "When Triggered?")
               (mMessageToSend, "Message To Send")
               (mIncrementMessage, "Increment (+1) Message")
               (mDecrementMessage, "Decrement (-1) Message")
               (mResetMessage, "Reset Message");
    }

    void CountDownToZero::onLoaded()
    {
        getLevelObject().setSpawnStrategy(od::SpawnStrategy::Always);
        getLevelObject().setObjectType(od::LevelObjectType::Detector);
    }

    void CountDownToZero::onSpawned()
    {
        mCounterValue = mInitialCounterValue;
    }

    void CountDownToZero::onMessageReceived(od::LevelObject &sender, od::Message message)
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
            getLevelObject().messageAllLinkedObjects(mMessageToSend);
        }

        Logger::verbose() << "Counter " << getLevelObject().getObjectId() << " received message '" << message << "' from " << sender.getObjectId() << ". "
                << "Count was " << mCounterValue << ", now is " << newCounter;

        mCounterValue = newCounter;
    }

}



