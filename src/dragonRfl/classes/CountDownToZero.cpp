/*
 * CountDownToZero.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/CountDownToZero.h>

#include <limits>
#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>

namespace dragonRfl
{

    CountDownToZero::CountDownToZero(DragonRfl &rfl)
    : mInitialCounterValue(1)
    , mWhenTriggered(TimerTriggerMode::DependsOnMessage)
    , mMessageToSend(odRfl::RflMessage::On)
    , mIncrementMessage(odRfl::RflMessage::Off)
    , mDecrementMessage(odRfl::RflMessage::On)
    , mResetMessage(odRfl::RflMessage::Triggered)
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

    void CountDownToZero::onLoaded(od::LevelObject &obj)
    {
        obj.setSpawnStrategy(od::SpawnStrategy::Always);
        obj.setObjectType(od::LevelObjectType::Detector);
    }

    void CountDownToZero::onSpawned(od::LevelObject &obj)
    {
        mCounterValue = mInitialCounterValue;
    }

    void CountDownToZero::onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message)
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


    OD_REGISTER_RFLCLASS(DragonRfl, CountDownToZero);

}



