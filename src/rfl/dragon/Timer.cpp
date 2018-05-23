/*
 * Timer.cpp
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */


#include "rfl/dragon/Timer.h"

#include "rfl/Rfl.h"

namespace odRfl
{

	Timer::Timer()
	: mTimeUntilTrigger(5.0)
	, mStartMode(1) // Run When Triggered
	, mRepeat(0)
	, mDestroyAfterTimeout(1)
	, mTriggerMessage(1) // On
	, mToggle(0)
	, mDisableReenableMessage(0) // Off
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

	void Timer::spawned(od::LevelObject &obj)
	{

	}


	OD_REGISTER_RFL_CLASS(0x003e, "Timer", Timer);

}
