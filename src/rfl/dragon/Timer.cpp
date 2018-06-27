/*
 * Timer.cpp
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */


#include "rfl/dragon/Timer.h"

#include "rfl/Rfl.h"
#include "LevelObject.h"

namespace odRfl
{

	Timer::Timer()
	: mTimeUntilTrigger(5.0)
	, mStartMode(TimerStartMode::RunWhenTriggered)
	, mRepeat(true)
	, mDestroyAfterTimeout(true)
	, mTriggerMessage(RflMessage::On)
	, mToggle(false)
	, mDisableReenableMessage(RflMessage::Off)
	, mTimerStarted(false)
	, mTimeElapsed(0.0)
	{
	}

	void Timer::probeFields(RflFieldProbe &probe)
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

	void Timer::loaded(od::Engine &engine, od::LevelObject *obj)
	{
	    if(obj == nullptr)
	    {
	        Logger::warn() << "Timer created without a level object";
	        return;
	    }

	    obj->setSpawnStrategy(od::SpawnStrategy::Always);
	}

	void Timer::spawned(od::LevelObject &obj)
	{
	    obj.setEnableRflUpdateHook(true);

	    mTimerStarted = (mStartMode == TimerStartMode::RunInstantly);
	}

	void Timer::update(od::LevelObject &obj, double simTime, double relTime)
	{
	    if(!mTimerStarted)
	    {
	        return;
	    }

	    mTimeElapsed += relTime;

	    if(mTimeElapsed >= mTimeUntilTrigger)
	    {
	        Logger::verbose() << "Timer " << obj.getObjectId() << " triggered at simTime=" << simTime << "s";

	        obj.messageAllLinkedObjects(mTriggerMessage);

	        mTimerStarted = false;

	        if(mDestroyAfterTimeout)
	        {
	            obj.requestDestruction();

	        }else if(mRepeat)
	        {
	            mTimerStarted = true;
	        }
	    }
	}

	void Timer::messageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message)
	{
	    if(!mTimerStarted)
	    {
	        // i assume any message will trigger the timer. there is no field that would indicate otherwise
	        mTimerStarted = true;
	    }
	}

	OD_REGISTER_RFL_CLASS(0x003e, "Timer", Timer);

}
