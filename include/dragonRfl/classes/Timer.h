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

namespace dragonRfl
{

    class DragonRfl;

    enum class TimerStartMode
    {
        RunInstantly,
        RunWhenTriggered
    };

    typedef odRfl::RflEnumImpl<TimerStartMode, 0, 1> RflEnumTimerStartMode;


	class Timer : public odRfl::RflClass
	{
	public:

		Timer(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onLoaded(od::LevelObject &obj) override;
        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onUpdate(od::LevelObject &obj, double simTime, double relTime) override;
        virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message) override;


	protected:

		odRfl::RflFloat 		mTimeUntilTrigger;
		RflEnumTimerStartMode   mStartMode;
		odRfl::RflEnumYesNo	    mRepeat;
		odRfl::RflEnumYesNo     mDestroyAfterTimeout;
		odRfl::RflEnumMessage   mTriggerMessage; // message that will be sent upon timeout
		odRfl::RflEnumYesNo     mToggle;
		odRfl::RflEnumMessage   mDisableReenableMessage;

	private:

		bool mGotStartTrigger;
		bool mTimerRunning;
		double mTimeElapsed;

	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x003e, "System", "Timer", dragonRfl::Timer);

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
