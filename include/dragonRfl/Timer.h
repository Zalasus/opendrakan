/*
 * Timer.h
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TIMER_H_
#define INCLUDE_RFL_DRAGON_TIMER_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

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
        virtual void onLoaded(od::LevelObject &obj) override;
        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onUpdate(od::LevelObject &obj, double simTime, double relTime) override;
        virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message) override;


	protected:

		RflFloat 		        mTimeUntilTrigger;
		RflEnumTimerStartMode   mStartMode;
		RflEnumYesNo	        mRepeat;
		RflEnumYesNo            mDestroyAfterTimeout;
		RflEnumMessage          mTriggerMessage; // message that will be sent upon timeout
		RflEnumYesNo            mToggle;
		RflEnumMessage          mDisableReenableMessage;

	private:

		bool mGotStartTrigger;
		bool mTimerRunning;
		double mTimeElapsed;

	};

}

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
