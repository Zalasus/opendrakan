/*
 * RflTimer.h
 *
 *  Created on: 1 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_GENERAL_RFLTIMER_H_
#define INCLUDE_RFL_GENERAL_RFLTIMER_H_

#include "rfl/RflClass.h"

namespace od
{

	class RflTimer : public RflClass
	{
	public:

		RflTimer();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawn(LevelObject &obj) override;


	protected:

		RflFloat 		mTimeUntilTrigger;
		RflEnum  		mStartMode;
		RflEnumYesNo	mRepeat;
		RflEnumYesNo    mDestroyAfterTimeout;
		RflEnum    		mTriggerMessage;
		RflEnumYesNo    mToggle;
		RflEnum			mDisableReenableMessage;

	};

}

#endif /* INCLUDE_RFL_GENERAL_RFLTIMER_H_ */
