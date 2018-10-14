/*
 * DragonControl.h
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_
#define INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_

#include <dragonRfl/classes/common/PlayerCommon.h>

namespace dragonRfl
{

    class DragonRfl;

	class DragonControl : public PlayerCommon
    {
    public:

        DragonControl(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;


    protected:

        odRfl::AnimRef			mWalkBackwardAnim;
		odRfl::Float			mAirCamHorzDistance;
		odRfl::Float			mAirCamVertDistance;
		odRfl::CharChannel		mSaddleChannel;
		odRfl::CharChannel		mJawChannel;
		odRfl::CharChannel		mNeck2Channel;
		odRfl::Integer			mBiteDamage;
		odRfl::SoundRefArray	mGotHitSounds;
		odRfl::SoundRefArray	mCrashSounds;
		odRfl::Integer			mCrashSoundChance;
		odRfl::SoundRefArray	mWingFlapSounds;
		odRfl::SoundRefArray	mBiteSounds;
		odRfl::SoundRefArray	mDoneMountingSounds;
		odRfl::Integer			mPercentChanceOfDoneMountingSound;
		odRfl::AnimRef			mAirDeathStartAnim;
		odRfl::AnimRef			mAirDeathLoopAnim;
		odRfl::AnimRef			mAirDeathImpactAnim;
		odRfl::AnimRef			mMountAnim;
		odRfl::AnimRef			mDismountAnim;
		odRfl::AnimRef			mFlyAnim;
		odRfl::AnimRef			mFlyToHoverAnim;
		odRfl::AnimRef			mHoverAnim;
		odRfl::AnimRef			mHoverBackwardsAnim;
		odRfl::AnimRef			mSoarAnim;
		odRfl::AnimRef			mStrafeLeftAirAnim;
		odRfl::AnimRef			mStrafeRightAirAnim;
		odRfl::AnimRef			mStrafeUpAnim;
		odRfl::AnimRef			mTurnLeftGndAnim;
		odRfl::AnimRef			mTurnRightGndAnim;
		odRfl::AnimRef			mLandingAnim;
		odRfl::AnimRef			mRunIntoWallAnim;
		odRfl::AnimRef			mFlyIntoWallAnim;
		odRfl::AnimRef			mBiteAnim;
		odRfl::Float			mMinFlySpeed;
		odRfl::Float			mMaxFlySpeed;
		odRfl::Float			mAirStrafeLRSpeed;
		odRfl::Float			mStartHoverAnimSpeed;
		odRfl::Float			mStartFlyAnimSpeed;
		odRfl::Float			mUpStrafeSpeed;
		odRfl::Float			mUpAccel;
		odRfl::Float			mTurnDamping;
		odRfl::Float			mClimbFactor;
		odRfl::Float			mDiveFactor;
		odRfl::Float			mSoarAngle;
		odRfl::Float			mNoFlyHeadroom;
		odRfl::Enum				mInitialMode; // Air, Ground Human, Ground Dragon
		odRfl::ClassRef			mDefaultWeapon;
		odRfl::Float			mFollowHumanDistance;
		odRfl::SoundRefArray	mCantLandSounds;
		odRfl::SoundRefArray	mCallResponseSounds;
		odRfl::SoundRefArray	mImCloseEnoughSounds;
		odRfl::SoundRefArray	mCantFlySounds;
		odRfl::SoundRefArray	mMeetAtCaveEntranceSounds;
		odRfl::SoundRefArray	mKilledEnemySounds;
		odRfl::SoundRefArray	mHealthLowSounds;
		odRfl::SoundRefArray	mGotHitByRynnSounds;
		odRfl::SoundRefArray	mGetOutOfMyWaySounds;
		odRfl::Float			mRechargeRatePerSecond;
		odRfl::SoundRefArray	mTakeOffSounds;
		odRfl::Integer			mTakeOffSoundChance;
		odRfl::SoundRefArray	mDoneDismountingSounds;
		odRfl::Integer			mDoneDismountSndChance;
    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x001c, "Player", "Dragon Control", dragonRfl::DragonControl);

#endif /* INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_ */
