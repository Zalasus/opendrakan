/*
 * Timer.h
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TIMER_H_
#define INCLUDE_RFL_DRAGON_TIMER_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>

namespace dragonRfl
{

    enum class TimerStartMode
    {
        RunInstantly,
        RunWhenTriggered
    };

    typedef odRfl::EnumImpl<TimerStartMode, 0, 1> EnumTimerStartMode;


	class Timer : public odRfl::LevelObjectClassBase
	{
	public:

		Timer();

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onUpdate(float relTime) override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


	protected:

		odRfl::Float 		 mTimeUntilTrigger;
		EnumTimerStartMode   mStartMode;
		odRfl::EnumYesNo	 mRepeat;
		odRfl::EnumYesNo     mDestroyAfterTimeout;
		odRfl::EnumMessage   mTriggerMessage; // message that will be sent upon timeout
		odRfl::EnumYesNo     mToggle;
		odRfl::EnumMessage   mDisableReenableMessage;

	private:

		bool mGotStartTrigger;
		bool mTimerRunning;
		float mTimeElapsed;

	};

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::Timer, 0x003e, "System", "Timer");

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
