/*
 * Building.h
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_BUILDING_H_
#define INCLUDE_RFL_DRAGON_BUILDING_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace od
{

	class Building : public RflClass
	{
	public:

		Building();

		virtual void probeFields(FieldProbe &probe) override;
        virtual void onSpawned(LevelObject &obj) override;
        virtual void onDespawned(LevelObject &obj) override;


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


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x0011, "Ground Object", "Building", Building);

}

#endif /* INCLUDE_RFL_DRAGON_BUILDING_H_ */
