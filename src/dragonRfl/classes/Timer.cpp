/*
 * Timer.cpp
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */


#include <dragonRfl/classes/Timer.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>

namespace dragonRfl
{

	Timer::Timer()
	: mTimeUntilTrigger(5.0)
	, mStartMode(TimerStartMode::RunWhenTriggered)
	, mRepeat(true)
	, mDestroyAfterTimeout(true)
	, mTriggerMessage(od::Message::On)
	, mToggle(false)
	, mDisableReenableMessage(od::Message::Off)
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

	void Timer::onLoaded()
	{
	    auto &obj = getLevelObject();
	    obj.setSpawnStrategy(od::SpawnStrategy::Always);
	    obj.setObjectType(od::LevelObjectType::Detector);
	}

	void Timer::onSpawned()
	{
        auto &obj = getLevelObject();

	    obj.setEnableRflUpdateHook(true);

	    mTimerRunning = (mStartMode == TimerStartMode::RunInstantly);
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
	    if(mStartMode == TimerStartMode::RunWhenTriggered && !mGotStartTrigger)
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
