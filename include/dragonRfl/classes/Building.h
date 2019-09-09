/*
 * Building.h
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_BUILDING_H_
#define INCLUDE_RFL_DRAGON_BUILDING_H_

#include <odCore/rfl/DefaultObjectClass.h>

#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace odRender
{
    class ObjectNode;
}

namespace odPhysics
{
    class ObjectHandle;
}

namespace dragonRfl
{

    class DragonRfl;

	class Building : public odRfl::DefaultObjectClass
	{
	public:

		Building();

		virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onSpawned() override;
        virtual void onDespawned() override;


	protected:

		odRfl::Integer			mInitialHealth;
		odRfl::Enum				mSnapMode;
		odRfl::ClassRef			mSoundEffectObject;
		odRfl::EnumYesNo		mIsDoorWay;
		odRfl::EnumYesNo		mCanArokhLandOn;
		odRfl::Enum				mDoorWayAlong;
		odRfl::EnumMessage		mMessageToSend;
		odRfl::ClassRef			m2DExplosionObject;
		odRfl::ClassRef			mExplosionGenerator;
		odRfl::ClassRef			mSmokeGenerator;
		odRfl::ModelRefArray	mRubbleAnimation;
		odRfl::Float			mRubbleAnimRate;
		odRfl::EnumYesNo		mFlammable;
		odRfl::Enum				mPushOverMode;
		odRfl::EnumYesNo		mFallWhenDead;
		odRfl::SoundRef			mHitGroundSound;
		odRfl::EnumYesNo		mSendMessageWhenPushed;
		odRfl::EnumMessage		mMessageToSendWhenPushed;
		odRfl::EnumYesNo		mSendMessageAfterPushed;
		odRfl::EnumMessage		mMessageToSendAfterPushed;

	};

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::Building, 0x0011, "Ground Object", "Building");

#endif /* INCLUDE_RFL_DRAGON_BUILDING_H_ */
