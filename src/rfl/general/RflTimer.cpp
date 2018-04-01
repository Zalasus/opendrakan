/*
 * RflTimer.cpp
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */


#include "rfl/general/RflTimer.h"

#include "rfl/Rfl.h"

namespace od
{

	RflTimer::RflTimer()
	: mTimeUntilTrigger(5.0)
	, mStartMode(1) // Run When Triggered
	, mRepeat(0)
	, mDestroyAfterTimeout(1)
	, mTriggerMessage(1) // On
	, mToggle(0)
	, mDisableReenableMessage(0) // Off
	{
	}

	void RflTimer::probeFields(RflFieldProbe &probe)
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

	void RflTimer::spawn(LevelObject &obj)
	{

	}


	OD_REGISTER_RFL_CLASS(0x003e, "Timer", RflTimer);

}
