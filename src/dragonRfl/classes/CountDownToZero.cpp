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

    CountDownToZeroFields::CountDownToZeroFields()
    : initialCounterValue(1)
    , whenTriggered(TimerTriggerMode::DependsOnMessage)
    , messageToSend(od::Message::On)
    , incrementMessage(od::Message::Off)
    , decrementMessage(od::Message::On)
    , resetMessage(od::Message::Triggered)
    {
    }

    void CountDownToZeroFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Count Down To Zero")
               (initialCounterValue, "Initial Counter Value")
               (whenTriggered, "When Triggered?")
               (messageToSend, "Message To Send")
               (incrementMessage, "Increment (+1) Message")
               (decrementMessage, "Decrement (-1) Message")
               (resetMessage, "Reset Message");
    }


    CountDownToZero_Sv::CountDownToZero_Sv()
    : mCounterValue(1)
    {
    }

    void CountDownToZero_Sv::onLoaded()
    {
        getLevelObject().setSpawnStrategy(od::SpawnStrategy::Always);
    }

    void CountDownToZero_Sv::onSpawned()
    {
        mCounterValue = mFields.initialCounterValue;
        //mStates.counterValue = mFields.initialCounterValue;
        //getLevelObject().extraStateDirty();
    }

    void CountDownToZero_Sv::onMessageReceived(od::LevelObject &sender, od::Message message)
    {
        uint32_t newCounter = mCounterValue;

        switch(mFields.whenTriggered)
        {
        case TimerTriggerMode::DecrementOnAnyMessage:
            if(newCounter > 0)
            {
                --newCounter;
            }
            break;

        case TimerTriggerMode::DependsOnMessage:
            if(message == mFields.incrementMessage)
            {
                if(newCounter < std::numeric_limits<uint32_t>::max())
                {
                    ++mCounterValue;
                }

            }else if(message == mFields.decrementMessage)
            {
                if(newCounter > 0)
                {
                    --newCounter;
                }

            }else if(message == mFields.resetMessage)
            {
                newCounter = mFields.initialCounterValue;
            }
            break;

        default:
            break;
        }

        // only trigger when we hit 0, not when we are already at zero when receiving message TODO: is this the right behaviour?
        if(mCounterValue != 0 && newCounter == 0)
        {
            getLevelObject().messageAllLinkedObjects(mFields.messageToSend);
        }

        Logger::verbose() << "Counter " << getLevelObject().getObjectId() << " received message '" << message << "' from " << sender.getObjectId() << ". "
                << "Count was " << mCounterValue << ", now is " << newCounter;

        mCounterValue = newCounter;
        getLevelObject().extraStateDirty();
    }

}
