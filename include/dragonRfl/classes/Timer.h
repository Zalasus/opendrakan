/*
 * Timer.h
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TIMER_H_
#define INCLUDE_RFL_DRAGON_TIMER_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>

namespace dragonRfl
{

    class DragonRfl;

    enum class TimerStartMode
    {
        RunInstantly,
        RunWhenTriggered
    };

    typedef odRfl::EnumImpl<TimerStartMode, 0, 1> EnumTimerStartMode;


	class Timer : public odRfl::RflClass
	{
	public:

		Timer(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded(od::LevelObject &obj) override;
        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onUpdate(od::LevelObject &obj, float relTime) override;
        virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message) override;


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

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x003e, "System", "Timer", dragonRfl::Timer);

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
