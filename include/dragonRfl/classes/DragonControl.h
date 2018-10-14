/*
 * DragonControl.h
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_
#define INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_

#include <dragonRfl/common/PlayerCommon.h>

namespace dragonRfl
{

    class DragonRfl;

	class DragonControl : public PlayerCommon
    {
    public:

        DragonControl(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;


    protected:

        odRfl::RflAnimRef			mWalkBackwardAnim;
		odRfl::RflFloat			    mAirCamHorzDistance;
		odRfl::RflFloat			    mAirCamVertDistance;
		odRfl::RflCharChannel		mSaddleChannel;
		odRfl::RflCharChannel		mJawChannel;
		odRfl::RflCharChannel		mNeck2Channel;
		odRfl::RflInteger			mBiteDamage;
		odRfl::RflSoundRefArray	    mGotHitSounds;
		odRfl::RflSoundRefArray	    mCrashSounds;
		odRfl::RflInteger			mCrashSoundChance;
		odRfl::RflSoundRefArray	    mWingFlapSounds;
		odRfl::RflSoundRefArray	    mBiteSounds;
		odRfl::RflSoundRefArray	    mDoneMountingSounds;
		odRfl::RflInteger			mPercentChanceOfDoneMountingSound;
		odRfl::RflAnimRef			mAirDeathStartAnim;
		odRfl::RflAnimRef			mAirDeathLoopAnim;
		odRfl::RflAnimRef			mAirDeathImpactAnim;
		odRfl::RflAnimRef			mMountAnim;
		odRfl::RflAnimRef			mDismountAnim;
		odRfl::RflAnimRef			mFlyAnim;
		odRfl::RflAnimRef			mFlyToHoverAnim;
		odRfl::RflAnimRef			mHoverAnim;
		odRfl::RflAnimRef			mHoverBackwardsAnim;
		odRfl::RflAnimRef			mSoarAnim;
		odRfl::RflAnimRef			mStrafeLeftAirAnim;
		odRfl::RflAnimRef			mStrafeRightAirAnim;
		odRfl::RflAnimRef			mStrafeUpAnim;
		odRfl::RflAnimRef			mTurnLeftGndAnim;
		odRfl::RflAnimRef			mTurnRightGndAnim;
		odRfl::RflAnimRef			mLandingAnim;
		odRfl::RflAnimRef			mRunIntoWallAnim;
		odRfl::RflAnimRef			mFlyIntoWallAnim;
		odRfl::RflAnimRef			mBiteAnim;
		odRfl::RflFloat			    mMinFlySpeed;
		odRfl::RflFloat			    mMaxFlySpeed;
		odRfl::RflFloat			    mAirStrafeLRSpeed;
		odRfl::RflFloat			    mStartHoverAnimSpeed;
		odRfl::RflFloat			    mStartFlyAnimSpeed;
		odRfl::RflFloat			    mUpStrafeSpeed;
		odRfl::RflFloat			    mUpAccel;
		odRfl::RflFloat			    mTurnDamping;
		odRfl::RflFloat			    mClimbFactor;
		odRfl::RflFloat			    mDiveFactor;
		odRfl::RflFloat			    mSoarAngle;
		odRfl::RflFloat			    mNoFlyHeadroom;
		odRfl::RflEnum				mInitialMode; // Air, Ground Human, Ground Dragon
		odRfl::RflClassRef			mDefaultWeapon;
		odRfl::RflFloat			    mFollowHumanDistance;
		odRfl::RflSoundRefArray	    mCantLandSounds;
		odRfl::RflSoundRefArray	    mCallResponseSounds;
		odRfl::RflSoundRefArray	    mImCloseEnoughSounds;
		odRfl::RflSoundRefArray	    mCantFlySounds;
		odRfl::RflSoundRefArray	    mMeetAtCaveEntranceSounds;
		odRfl::RflSoundRefArray	    mKilledEnemySounds;
		odRfl::RflSoundRefArray	    mHealthLowSounds;
		odRfl::RflSoundRefArray	    mGotHitByRynnSounds;
		odRfl::RflSoundRefArray	    mGetOutOfMyWaySounds;
		odRfl::RflFloat			    mRechargeRatePerSecond;
		odRfl::RflSoundRefArray	    mTakeOffSounds;
		odRfl::RflInteger			mTakeOffSoundChance;
		odRfl::RflSoundRefArray	    mDoneDismountingSounds;
		odRfl::RflInteger			mDoneDismountSndChance;
    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x001c, "Player", "Dragon Control", dragonRfl::DragonControl);

#endif /* INCLUDE_RFL_DRAGON_DRAGONCONTROL_H_ */
