/*
 * Timer.cpp
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */


#include <dragonRfl/classes/Timer.h>

#include <odCore/LevelObject.h>

namespace dragonRfl
{

	TimerFields::TimerFields()
	: timeUntilTrigger(5.0)
	, startMode(TimerStartMode::RUN_WHEN_TRIGGERED)
	, repeat(true)
	, destroyAfterTimeout(true)
	, triggerMessage(od::Message::On)
	, toggle(false)
	, disableReenableMessage(od::Message::Off)
	{
	}

	void TimerFields::probeFields(odRfl::FieldProbe &probe)
	{
		probe("Timer")
				(timeUntilTrigger, "Time Until Trigger")
				(startMode, "Start Mode")
				(repeat, "Repeat")
				(destroyAfterTimeout, "Destroy After Timeout?")
				(triggerMessage, "Trigger Message")
				(toggle, "Toggle (Message / Off)")
				(disableReenableMessage, "Disable/Re-Enable Message");
	}


	Timer_Sv::Timer_Sv()
	: mGotStartTrigger(false)
    , mTimerRunning(false)
    , mTimeElapsed(0.0)
	{
	}

	void Timer_Sv::onLoaded()
	{
	    auto &obj = getLevelObject();
	    obj.setSpawnStrategy(od::SpawnStrategy::Always);
	}

	void Timer_Sv::onSpawned()
	{
        auto &obj = getLevelObject();

	    obj.setEnableUpdate(true);

	    mTimerRunning = (mFields.startMode == TimerStartMode::RUN_INSTANTLY);
	}

	void Timer_Sv::onUpdate(float relTime)
	{
        auto &obj = getLevelObject();

	    if(!mTimerRunning)
	    {
	        return;
	    }

	    mTimeElapsed += relTime;

	    if(mTimeElapsed >= mFields.timeUntilTrigger)
	    {
	        Logger::verbose() << "Timer " << obj.getObjectId() << " triggered after " << mTimeElapsed << "s";

	        obj.messageAllLinkedObjects(mFields.triggerMessage);

	        mTimerRunning = false;

	        if(mFields.destroyAfterTimeout)
	        {
	            obj.requestDestruction();

	        }else if(mFields.repeat)
	        {
	            mTimerRunning = true;
	            mTimeElapsed = 0.0;
	        }
	    }
	}

	void Timer_Sv::onMessageReceived(od::LevelObject &sender, od::Message message)
	{
	    if(mFields.startMode == TimerStartMode::RUN_WHEN_TRIGGERED && !mGotStartTrigger)
	    {
	        // i assume any message will trigger the timer. there is no field that would indicate otherwise
	        mTimerRunning = true;
	        mGotStartTrigger = true;

	        Logger::verbose() << "Timer " << getLevelObject().getObjectId() << " started by object " << sender.getObjectId();

	    }else if(mFields.toggle && message == mFields.disableReenableMessage)
	    {
	        mTimerRunning = !mTimerRunning;

	        Logger::verbose() << "Timer " << getLevelObject().getObjectId() << " " << (mTimerRunning ? "enabled" : "disabled")
	                          << " by object " << sender.getObjectId();
	    }
	}

}
