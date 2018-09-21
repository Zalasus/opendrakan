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

namespace od
{

    class DragonRfl;

    enum class TimerStartMode
    {
        RunInstantly,
        RunWhenTriggered
    };

    typedef RflEnumImpl<TimerStartMode, 0, 1> RflEnumTimerStartMode;


	class Timer : public RflClass
	{
	public:

		Timer(DragonRfl &rfl);

        virtual void probeFields(FieldProbe &probe) override;
        virtual void onLoaded(LevelObject &obj) override;
        virtual void onSpawned(LevelObject &obj) override;
        virtual void onUpdate(LevelObject &obj, double simTime, double relTime) override;
        virtual void onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message) override;


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


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x003e, "System", "Timer", Timer);

}

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
