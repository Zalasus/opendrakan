/*
 * RflPlayerControl.h
 *
 *  Created on: 16 Feb 2018
 *      Author: zal
 */

#ifndef RFLPLAYERCONTROL_H_
#define RFLPLAYERCONTROL_H_

#include "rfl/RflClass.h"

namespace od
{

    class RflPlayerCommon : public RflClass
    {
    public:

        RflPlayerCommon();

        virtual void probeFields(RflFieldProbe &probe) override;


    protected:

        RflSoundRefArray    mSeparatedSounds;
        RflInteger          mSeparatedSoundChance;
        RflFloat            mCameraHorzDistance;
        RflFloat            mCameraVertDistance;
        RflFloat            mCameraTiltFactor;
        RflFloat            mCameraCollisionTiltFactor;
        RflFloat            mMaxProximity;
        RflAnimRef          mReadyAnim;
        RflAnimRef          mWalkAnim;
        RflAnimRef          mRunAnim;
        RflAnimRef          mGroundStrafeLAnim;
        RflAnimRef          mGroundStrafeRAnim;
        RflAnimRefArray     mIdleAnims;
        RflFloat            mRunSpeed;
        RflFloat            mWalkSpeed;
        RflFloat            mWalkBackSpeed;
        RflFloat            mThrottleAccel;
        RflFloat            mThrottleDecel;
        RflFloat            mSideStrafeSpeed;
        RflFloat            mRollSpeed;
        RflFloat            mPitchSpeed;
        RflFloat            mSideAccel;
        RflFloat            mRollDamping;
        RflFloat            mPitchDamping;
        RflCharChannel      mNeckChannel;
        RflCharChannel      mHeadChannel;
        RflInteger          mFootSphere;
        RflInteger          mBeginHealth;
        RflClassRef         mGotHitBloodGen;
        RflAnimRefArray     mGroundDeathAnims;
        RflClassRef         m3DExplosion;
        RflSoundRef         mDeathSound;
        RflClassRef         mAmputationBloodGen;
        RflClassRef         mBodyPart;
        RflClassRef         mBodyPartLarge;
        RflFloat            mBodyPartPercentage;
        RflCharChannelArray mChoppableChansAlive;
        RflInteger          mGibCutoff;
        RflClassRefArray    mGibGensLarge;
        RflClassRefArray    mGibGensSmall;
        RflCharChannelArray mLargeGibChans;
        RflFloat            mBodyPartVelocity;
    };


    class RflDragonControl : public RflPlayerCommon
    {
    public:

        RflDragonControl();

        virtual void probeFields(RflFieldProbe &probe) override;


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


    class RflHumanControl : public RflPlayerCommon
    {
    public:

        RflHumanControl();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawn(LevelObject &obj) override;


    protected:

        // 179 members, woo! TODO: maybe put them in an array or something? would need to extend our reflection concept a bit
        /*RflCharChannel		mRightWeaponChannel;
		RflCharChannel		mLeftWeaponChannel;
		RflCharChannel		mTorsoChannel;
		RflCharChannel		mLowerBodyChan;
		RflCharChannel		mRightArmChan;
		RflCharChannelArray	mSolveToChans;
		RflCharChannelArray	mLeftLegChans;
		RflCharChannelArray	mRightLegChans;
		RflCharChannel		mHair1Channel;
		RflFloat		    mMultiplayerSpeedScaling;
		RflFloat		    mFallStartDmgSpeed;
		RflFloat		    mFallDeathSpeed;
		RflAnimRef		    mDrown;
		RflSoundRef		    mDrownSound;
		RflAnimRef		    mCombatIdleOneHand;
		RflAnimRef		    mCombatIdleTwoHand;
		RflAnimRef		    mWalkOneHanded;
		RflAnimRef		    mRunOneHanded;
		RflAnimRef		    mStrafeLeftOneHanded;
		RflAnimRef		    mStrafeRightOneHanded;
		RflAnimRef		    mWalkCombat;
		RflAnimRef		    mRunCombat;
		RflAnimRef		    mRideAir;
		RflAnimRef		    mRideGround;
		RflAnimRef		    mMount;
		RflAnimRef		    mDismount;
		RflAnimRef		    mJump;
		RflAnimRef		    mInAir;
		RflAnimRef		    mLanding;
		RflAnimRef		    mLandingSmall;
		RflAnimRef		    mLandingRun;
		RflAnimRef		    mNoncombatToCrouch;
		RflAnimRef		    mCombatToCrouch;
		RflAnimRef		    mNoncombatCrouched;
		RflAnimRef		    mCombatCrouched;
		RflAnimRef		    mWaddleForward;
		RflAnimRef		    mRollLeft;
		RflAnimRef		    mRollRight;
		RflAnimRef		    mRollForward;
		RflAnimRef		    mRollBack;
		RflAnimRef		    mCartwheelL;
		RflAnimRef		    mCartwheelR;
		RflAnimRef		    mFlipForward;
		RflAnimRef		    mFlipBack;
		RflAnimRef		    mPushRollable;
		RflAnimRef		    mPushSlidable;
		RflAnimRef		    mLadderUp1;
		RflAnimRef		    mLadderUp2;
		RflAnimRef		    mLadderDown1;
		RflAnimRef		    mLadderDown2;
		RflAnimRef		    mLadderUpLIdle;
		RflAnimRef		    mLadderUpRIdle;
		RflAnimRef		    mLadderDownRIdle;
		RflAnimRef		    mGetOnLadderB;
		RflAnimRef		    mGetOffLadderB;
		RflAnimRef		    mGetOnLadderT;
		RflAnimRef		    mGetOffLadderT;
		RflAnimRef		    mStairsUp;
		RflAnimRef		    mStairsDown;
		RflAnimRef		    mStairsUpIdleL;
		RflAnimRef		    mStairsUpIdleR;
		RflAnimRef		    mStairsDownIdleL;
		RflAnimRef		    mStairsDownIdleR;
		RflAnimRef		    mHuh;
		RflAnimRef		    m180Turn;
		RflAnimRef		    mUseMagicCrystal;
		RflAnimRef		    mSlideForward;
		RflAnimRef		    mSlideBackward;
		RflAnimRef		    mBalancing;
		RflAnimRef		    mThrow;
		RflAnimRef		    mShootGun;
		RflAnimRef		    mReloadGun;
		RflAnimRef		    mArcherMedium;
		RflAnimRef		    mArcherLow;
		RflFloat		    mGunCamHorzDistance;
		RflFloat		    mGunCamVertDistance;
		RflAnimRef		    mOneHandRH;
		RflAnimRef		    mOneHandRM1;
		RflAnimRef		    mOneHandRM2;
		RflAnimRef		    mOneHandRM3;
		RflAnimRef		    mOneHandRL;
		RflAnimRef		    mOneHandSH;
		RflAnimRef		    mOneHandSM;
		RflAnimRef		    mOneHandSL;
		RflAnimRef		    mOneHandUB1;
		RflAnimRef		    mOneHandUB2;
		RflAnimRef		    mOneHandUB3;
		RflAnimRef		    mTwoHandRH;
		RflAnimRef		    mTwoHandRM1;
		RflAnimRef		    mTwoHandRM2;
		RflAnimRef		    mTwoHandRM3;
		RflAnimRef		    mTwoHandRL;
		RflAnimRef		    mTwoHandSH;
		RflAnimRef		    mTwoHandSM;
		RflAnimRef		    mTwoHandSL;
		RflAnimRef		    mTwoHandUB1;
		RflAnimRef		    mTwoHandUB2;
		RflAnimRef		    mTwoHandUB3;
		RflAnimRef		    mFireSA;
		RflAnimRef		    mIceSA;
		RflAnimRef		    mLightningSA;
		RflAnimRef		    mDeathSA;
		RflAnimRef		    mStrafeAttackLUB;
		RflAnimRef		    mStrafeAttackRUB;
		RflAnimRef		    mRollLAttack;
		RflAnimRef		    mRollRAttack;
		RflAnimRef		    mSpecialAttackThrust;
		RflAnimRef		    mSpecialAttackLeft;
		RflAnimRef		    mSpecialAttackRight;
		RflAnimRef		    mSpecialAttackSpin;
		RflAnimRef		    mSpecialAttackSwing180;
		RflAnimRef		    mSpecialAttackJump;
		RflFloat		    mSAThrustDamageMultiplier;
		RflFloat		    mSALeftDamageMultiplier;
		RflFloat		    mSARightDamageMultiplier;
		RflFloat		    mSASpinDamageMultiplier;
		RflFloat		    mSASwing180DamageMultiplier;
		RflFloat		    mSAJumpDamageMultiplier;
		RflAnimRef		    mBlock;
		RflAnimRef		    mCrouchedBlock;
		RflInteger		    mBlockDamageReductionPercent;
		RflAnimRef		    mSheathe;
		RflAnimRef		    mUnsheathe;
		RflFloat		    mReAttackWindow;
		RflAnimRef		    mHitRxnHSLAnim;
		RflAnimRef		    mHitRxnHSHAnim;
		RflAnimRef		    mHitRxnOSLAnim;
		RflAnimRef		    mHitRxnOSHAnim;
		RflAnimRef		    mHitRxnFSLAnim;
		RflAnimRef		    mHitRxnFSHAnim;
		RflSoundRefArray	mHitRxnHSLSound;
		RflSoundRefArray	mHitRxnHSHSound;
		RflSoundRefArray	mHitRxnOSLSound;
		RflSoundRefArray	mHitRxnOSHSound;
		RflSoundRefArray	mHitRxnFSLSound;
		RflSoundRefArray	mHitRxnFSHSound;
		RflAnimRef		    mWalkStrafeL;
		RflAnimRef		    mWalkStrafeR;
		RflAnimRef		    mSlowWalk;
		RflAnimRef		    mRunBackwards;
		RflAnimRef		    mRunBackwardsOneHanded;
		RflAnimRef		    mNoncombat2Run;
		RflAnimRef		    mRun2Ready;
		RflAnimRef		    mRun2ReadyL;
		RflAnimRef		    mWalk2Ready;
		RflAnimRef		    mStrafeL2Ready;
		RflAnimRef		    mStrafeR2Ready;
		RflAnimRef		    mTurnLeft;
		RflAnimRef		    mTurnRight;
		RflAnimRef		    mBumpIntoWall;
		RflFloat		    mSwimForwardSpeed;
		RflFloat		    mSwimBackwardSpeed;
		RflFloat		    mAirConsumptionRate;
		RflFloat		    mAirReplenishmentRate;
		RflAnimRef		    mInAirToSwanDive;
		RflAnimRef		    mSwanDive;
		RflAnimRef		    mSwim;
		RflAnimRef		    mBackswim;
		RflAnimRef		    mSwimToBackswim;
		RflAnimRef		    mBackswimToSwim;
		RflAnimRef		    mTreadWater;
		RflAnimRef		    mIdleUnderwater;
		RflAnimRef		    mIdleUWtoTread;
		RflAnimRef		    mSwimToRun;
		RflSoundRef		    mSlideSound;
		RflSoundRef		    mHardLandingSound;
		RflSoundRefArray	mDragonCallSounds;
		RflSoundRefArray	mHuhSounds;
		RflSoundRefArray	mCantReachSounds;
		RflSoundRefArray	mCantUseSounds;
		RflSoundRef			mFallIntoWaterSound;
		RflSoundRef			mInventoryFullSound;
		RflSoundRef			mNoDartsSound;
		RflSoundRef			mNoArrowsSound;
		RflSoundRef			mSheatheSound;
		RflSoundRefArray	mPushObjectSounds;
		RflSoundRefArray	mAttackGrunts;
		RflSoundRefArray	mGetThePointSounds;
		RflInteger			mGetThePointSndChance;*/
    };
}

#endif /* RFLPLAYERCONTROL_H_ */
