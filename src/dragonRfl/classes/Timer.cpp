/*
 * Timer.cpp
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */


#include <dragonRfl/classes/Timer.h>

#include <odCore/LevelObject.h>

#include <dragonRfl/RflDragon.h>

namespace dragonRfl
{

	TimerFields::TimerFields()
	: mTimeUntilTrigger(5.0)
	, mStartMode(TimerStartMode::RUN_WHEN_TRIGGERED)
	, mRepeat(true)
	, mDestroyAfterTimeout(true)
	, mTriggerMessage(od::Message::On)
	, mToggle(false)
	, mDisableReenableMessage(od::Message::Off)
	{
	}

	void TimerFields::probeFields(odRfl::FieldProbe &probe)
	{
		probe("Timer")
				(mTimeUntilTrigger, "Time Until Trigger")
				(mStartMode, "Start Mode")
				(mRepeat, "Repeat")
				(mDestroyAfterTimeout, "Destroy After Timeout?")
				(mTriggerMessage, "Trigger Message")
				(mToggle, "Toggle (Message / Off)")
				(mDisableReenableMessage, "Disable/Re-Enable Message");
	}


	Timer::Timer()
	: mGotStartTrigger(false)
    , mTimerRunning(false)
    , mTimeElapsed(0.0)
	{
	}

	void Timer::onLoaded()
	{
	    auto &obj = getLevelObject();
	    obj.setSpawnStrategy(od::SpawnStrategy::Always);
	    obj.setObjectType(od::LevelObjectType::Detector);
	}

	void Timer::onSpawned()
	{
        auto &obj = getLevelObject();

	    obj.setEnableUpdate(true);

	    mTimerRunning = (mStartMode == TimerStartMode::RUN_INSTANTLY);
	}

	void Timer::onUpdate(float relTime)
	{
        auto &obj = getLevelObject();

	    if(!mTimerRunning)
	    {
	        return;
	    }

	    mTimeElapsed += relTime;

	    if(mTimeElapsed >= mTimeUntilTrigger)
	    {
	        Logger::verbose() << "Timer " << obj.getObjectId() << " triggered after " << mTimeElapsed << "s";

	        obj.messageAllLinkedObjects(mTriggerMessage);

	        mTimerRunning = false;

	        if(mDestroyAfterTimeout)
	        {
	            obj.requestDestruction();

	        }else if(mRepeat)
	        {
	            mTimerRunning = true;
	            mTimeElapsed = 0.0;
	        }
	    }
	}

	void Timer::onMessageReceived(od::LevelObject &sender, od::Message message)
	{
	    if(mStartMode == TimerStartMode::RUN_WHEN_TRIGGERED && !mGotStartTrigger)
	    {
	        // i assume any message will trigger the timer. there is no field that would indicate otherwise
	        mTimerRunning = true;
	        mGotStartTrigger = true;

	        Logger::verbose() << "Timer " << getLevelObject().getObjectId() << " started by object " << sender.getObjectId();

	    }else if(mToggle && message == mDisableReenableMessage)
	    {
	        mTimerRunning = !mTimerRunning;

	        Logger::verbose() << "Timer " << getLevelObject().getObjectId() << " " << (mTimerRunning ? "enabled" : "disabled")
	                          << " by object " << sender.getObjectId();
	    }
	}

}
