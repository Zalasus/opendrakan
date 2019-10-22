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

    class TimerFields : public odRfl::FieldBundle
    {
    public:

        enum class TimerStartMode
        {
            RUN_INSTANTLY,
            RUN_WHEN_TRIGGERED
        };
        typedef odRfl::EnumImpl<TimerStartMode, 0, 1> EnumTimerStartMode;


        TimerFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override final;


    protected:

        odRfl::Float         mTimeUntilTrigger;
        EnumTimerStartMode   mStartMode;
        odRfl::EnumYesNo     mRepeat;
        odRfl::EnumYesNo     mDestroyAfterTimeout;
        odRfl::EnumMessage   mTriggerMessage; // message that will be sent upon timeout
        odRfl::EnumYesNo     mToggle;
        odRfl::EnumMessage   mDisableReenableMessage;
    };


	class Timer : public odRfl::ClassBase, odRfl::Spawnable, odRfl::ClassImpl, TimerFields
	{
	public:

		Timer();

		virtual odRfl::Spawnable *asSpawnable() override { return this; }
		virtual odRfl::FieldBundle *getFieldBundle() override { return this; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onUpdate(float relTime) override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


	private:

		bool mGotStartTrigger;
		bool mTimerRunning;
		float mTimeElapsed;

	};

}

ODRFL_DEFINE_CLASS(dragonRfl::Timer, 0x003e, "System", "Timer");
ODRFL_DEFINE_CLASS_FIELDS(dragonRfl::Timer, dragonRfl::TimerFields);

// the timer base class provides it's own implementation
ODRFL_DEFINE_CLASS_IMPL_SERVER(dragonRfl::Timer, dragonRfl::Timer);

// timers are generally server-only
//ODRFL_DEFINE_CLASS_IMPL_CLIENT(dragonRfl::Timer, dragonRfl::Timer);

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
