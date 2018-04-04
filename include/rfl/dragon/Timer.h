/*
 * Timer.h
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_TIMER_H_
#define INCLUDE_RFL_DRAGON_TIMER_H_

#include "rfl/RflClass.h"

namespace odRfl
{

	class Timer : public RflClass
	{
	public:

		Timer();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawn(od::LevelObject &obj) override;


	protected:

		RflFloat 		mTimeUntilTrigger;
		RflEnum  		mStartMode;
		RflEnumYesNo	mRepeat;
		RflEnumYesNo    mDestroyAfterTimeout;
		RflEnumMessage  mTriggerMessage;
		RflEnumYesNo    mToggle;
		RflEnumMessage  mDisableReenableMessage;

	};

}

#endif /* INCLUDE_RFL_DRAGON_TIMER_H_ */
