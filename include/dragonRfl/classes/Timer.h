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
#include <odCore/rfl/DummyClass.h>

#include <dragonRfl/classes/common/Enums.h>

namespace dragonRfl
{

    struct TimerFields : public odRfl::FieldBundle
    {
        TimerFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override final;

        odRfl::Float         timeUntilTrigger;
        EnumStartMode        startMode;
        odRfl::EnumYesNo     repeat;
        odRfl::EnumYesNo     destroyAfterTimeout;
        odRfl::EnumMessage   triggerMessage; // message that will be sent upon timeout
        odRfl::EnumYesNo     toggle;
        odRfl::EnumMessage   disableReenableMessage;
    };


	class Timer_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<Timer_Sv>
	{
	public:

		Timer_Sv();

		virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onUpdate(float relTime) override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


	private:

        TimerFields mFields;

		bool mGotStartTrigger;
		bool mTimerRunning;
		float mTimeElapsed;

	};


    using TimerFactory = odRfl::ServerOnlyClassFactory<TimerFields, Timer_Sv>;

    OD_DEFINE_CLASS(Timer, 0x003e, "System", "Timer", TimerFactory);

}

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
