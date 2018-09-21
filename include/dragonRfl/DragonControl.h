/*
 * DragonControl.h
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_
#define INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_

#include <dragonRfl/common/PlayerCommon.h>

namespace od
{

	class DragonControl : public PlayerCommon
    {
    public:

        DragonControl();

        virtual void probeFields(FieldProbe &probe) override;


    protected:

        RflAnimRef			mWalkBackwardAnim;
		RflFloat			mAirCamHorzDistance;
		RflFloat			mAirCamVertDistance;
		RflCharChannel		mSaddleChannel;
		RflCharChannel		mJawChannel;
		RflCharChannel		mNeck2Channel;
		RflInteger			mBiteDamage;
		RflSoundRefArray	mGotHitSounds;
		RflSoundRefArray	mCrashSounds;
		RflInteger			mCrashSoundChance;
		RflSoundRefArray	mWingFlapSounds;
		RflSoundRefArray	mBiteSounds;
		RflSoundRefArray	mDoneMountingSounds;
		RflInteger			mPercentChanceOfDoneMountingSound;
		RflAnimRef			mAirDeathStartAnim;
		RflAnimRef			mAirDeathLoopAnim;
		RflAnimRef			mAirDeathImpactAnim;
		RflAnimRef			mMountAnim;
		RflAnimRef			mDismountAnim;
		RflAnimRef			mFlyAnim;
		RflAnimRef			mFlyToHoverAnim;
		RflAnimRef			mHoverAnim;
		RflAnimRef			mHoverBackwardsAnim;
		RflAnimRef			mSoarAnim;
		RflAnimRef			mStrafeLeftAirAnim;
		RflAnimRef			mStrafeRightAirAnim;
		RflAnimRef			mStrafeUpAnim;
		RflAnimRef			mTurnLeftGndAnim;
		RflAnimRef			mTurnRightGndAnim;
		RflAnimRef			mLandingAnim;
		RflAnimRef			mRunIntoWallAnim;
		RflAnimRef			mFlyIntoWallAnim;
		RflAnimRef			mBiteAnim;
		RflFloat			mMinFlySpeed;
		RflFloat			mMaxFlySpeed;
		RflFloat			mAirStrafeLRSpeed;
		RflFloat			mStartHoverAnimSpeed;
		RflFloat			mStartFlyAnimSpeed;
		RflFloat			mUpStrafeSpeed;
		RflFloat			mUpAccel;
		RflFloat			mTurnDamping;
		RflFloat			mClimbFactor;
		RflFloat			mDiveFactor;
		RflFloat			mSoarAngle;
		RflFloat			mNoFlyHeadroom;
		RflEnum				mInitialMode; // Air, Ground Human, Ground Dragon
		RflClassRef			mDefaultWeapon;
		RflFloat			mFollowHumanDistance;
		RflSoundRefArray	mCantLandSounds;
		RflSoundRefArray	mCallResponseSounds;
		RflSoundRefArray	mImCloseEnoughSounds;
		RflSoundRefArray	mCantFlySounds;
		RflSoundRefArray	mMeetAtCaveEntranceSounds;
		RflSoundRefArray	mKilledEnemySounds;
		RflSoundRefArray	mHealthLowSounds;
		RflSoundRefArray	mGotHitByRynnSounds;
		RflSoundRefArray	mGetOutOfMyWaySounds;
		RflFloat			mRechargeRatePerSecond;
		RflSoundRefArray	mTakeOffSounds;
		RflInteger			mTakeOffSoundChance;
		RflSoundRefArray	mDoneDismountingSounds;
		RflInteger			mDoneDismountSndChance;
    };


    OD_DECLARE_RFLCLASS_TRAITS(DragonRfl, 0x001c, "Player", "Dragon Control", DragonControl);

}

#endif /* INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_ */
