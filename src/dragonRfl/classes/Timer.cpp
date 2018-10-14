/*
 * Timer.cpp
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */


#include <dragonRfl/Timer.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>

namespace dragonRfl
{

	Timer::Timer(DragonRfl &rfl)
	: mTimeUntilTrigger(5.0)
	, mStartMode(TimerStartMode::RunWhenTriggered)
	, mRepeat(true)
	, mDestroyAfterTimeout(true)
	, mTriggerMessage(odRfl::RflMessage::On)
	, mToggle(false)
	, mDisableReenableMessage(odRfl::RflMessage::Off)
	, mGotStartTrigger(false)
	, mTimerRunning(false)
	, mTimeElapsed(0.0)
	{
	}

	void Timer::probeFields(odRfl::FieldProbe &probe)
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

	void Timer::onLoaded(od::LevelObject &obj)
	{
	    obj.setSpawnStrategy(od::SpawnStrategy::Always);
	    obj.setObjectType(od::LevelObjectType::Detector);
	}

	void Timer::onSpawned(od::LevelObject &obj)
	{
	    obj.setEnableRflUpdateHook(true);

	    mTimerRunning = (mStartMode == TimerStartMode::RunInstantly);
	}

	void Timer::onUpdate(od::LevelObject &obj, double simTime, double relTime)
	{
	    if(!mTimerRunning)
	    {
	        return;
	    }

	    mTimeElapsed += relTime;

	    if(mTimeElapsed >= mTimeUntilTrigger)
	    {
	        Logger::verbose() << "Timer " << obj.getObjectId() << " triggered at simTime=" << simTime << "s";

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

	void Timer::onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message)
	{
	    if(mStartMode == TimerStartMode::RunWhenTriggered && !mGotStartTrigger)
	    {
	        // i assume any message will trigger the timer. there is no field that would indicate otherwise
	        mTimerRunning = true;
	        mGotStartTrigger = true;

	        Logger::verbose() << "Timer " << obj.getObjectId() << " started by object " << sender.getObjectId();

	    }else if(mToggle && message == mDisableReenableMessage)
	    {
	        mTimerRunning = !mTimerRunning;

	        Logger::verbose() << "Timer " << obj.getObjectId() << " " << (mTimerRunning ? "enabled" : "disabled")
	                          << " by object " << sender.getObjectId();
	    }
	}


	OD_REGISTER_RFLCLASS(DragonRfl, Timer);

}
