/*
 * Building.h
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_BUILDING_H_
#define INCLUDE_RFL_DRAGON_BUILDING_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"

namespace odRfl
{

	class Building : public RflClass
	{
	public:

		Building();

		virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawn(od::LevelObject &obj) override;


	protected:

		RflInteger			mInitialHealth;
		RflEnum				mSnapMode;
		RflClassRef			mSoundEffectObject;
		RflEnumYesNo		mIsDoorWay;
		RflEnumYesNo		mCanArokhLandOn;
		RflEnum				mDoorWayAlong;
		RflEnumMessage		mMessageToSend;
		RflClassRef			m2DExplosionObject;
		RflClassRef			mExplosionGenerator;
		RflClassRef			mSmokeGenerator;
		RflModelRefArray	mRubbleAnimation;
		RflFloat			mRubbleAnimRate;
		RflEnumYesNo		mFlammable;
		RflEnum				mPushOverMode;
		RflEnumYesNo		mFallWhenDead;
		RflSoundRef			mHitGroundSound;
		RflEnumYesNo		mSendMessageWhenPushed;
		RflEnumMessage		mMessageToSendWhenPushed;
		RflEnumYesNo		mSendMessageAfterPushed;
		RflEnumMessage		mMessageToSendAfterPushed;
	};

}

#endif /* INCLUDE_RFL_DRAGON_BUILDING_H_ */
