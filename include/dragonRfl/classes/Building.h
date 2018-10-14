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

namespace dragonRfl
{

    class DragonRfl;

	class Building : public odRfl::RflClass
	{
	public:

		Building(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onDespawned(od::LevelObject &obj) override;


	protected:

		odRfl::RflInteger			mInitialHealth;
		odRfl::RflEnum				mSnapMode;
		odRfl::RflClassRef			mSoundEffectObject;
		odRfl::RflEnumYesNo		mIsDoorWay;
		odRfl::RflEnumYesNo		mCanArokhLandOn;
		odRfl::RflEnum				mDoorWayAlong;
		odRfl::RflEnumMessage		mMessageToSend;
		odRfl::RflClassRef			m2DExplosionObject;
		odRfl::RflClassRef			mExplosionGenerator;
		odRfl::RflClassRef			mSmokeGenerator;
		odRfl::RflModelRefArray	mRubbleAnimation;
		odRfl::RflFloat			mRubbleAnimRate;
		odRfl::RflEnumYesNo		mFlammable;
		odRfl::RflEnum				mPushOverMode;
		odRfl::RflEnumYesNo		mFallWhenDead;
		odRfl::RflSoundRef			mHitGroundSound;
		odRfl::RflEnumYesNo		mSendMessageWhenPushed;
		odRfl::RflEnumMessage		mMessageToSendWhenPushed;
		odRfl::RflEnumYesNo		mSendMessageAfterPushed;
		odRfl::RflEnumMessage		mMessageToSendAfterPushed;
	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0011, "Ground Object", "Building", dragonRfl::Building);

#endif /* INCLUDE_RFL_DRAGON_BUILDING_H_ */
