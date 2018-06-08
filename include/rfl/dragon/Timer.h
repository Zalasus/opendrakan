/*
 * Timer.h
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TIMER_H_
#define INCLUDE_RFL_DRAGON_TIMER_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"

namespace odRfl
{

    enum class TimerStartMode
    {
        RunInstantly,
        RunWhenTriggered
    };

    typedef RflEnumImpl<TimerStartMode, 0, 1> RflEnumTimerStartMode;


	class Timer : public RflClass
	{
	public:

		Timer();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawned(od::LevelObject &obj) override;
        virtual void update(od::LevelObject &obj, double simTime, double relTime) override;
        virtual void messageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message) override;


	protected:

		RflFloat 		        mTimeUntilTrigger;
		RflEnumTimerStartMode   mStartMode;
		RflEnumYesNo	        mRepeat;
		RflEnumYesNo            mDestroyAfterTimeout;
		RflEnumMessage          mTriggerMessage;
		RflEnumYesNo            mToggle;
		RflEnumMessage          mDisableReenableMessage;

	private:

		bool mTimerStarted;
		double mTimeElapsed;

	};

}

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
