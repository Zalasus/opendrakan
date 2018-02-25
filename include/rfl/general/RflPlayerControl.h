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


    protected:/*
        channel		mRightWeaponChannel;
		channel		mLeftWeaponChannel;
		channel		mTorsoChannel;
		channel		mLowerBodyChan;
		channel		mRightArmChan
		channel[]	mSolveToChans;
		channel[]	mLeftLegChans;
		channel[]	mRight Leg Chans
		channel		mHair1 Channel
		float		mMultiplayer Speed Scaling
		float		mFall Start Dmg Speed (lu/s)
		float		mFall Death Speed (lu/s)
		anim		mDrown
		sound		mDrown Sound
		anim		mCombat Idle OneHand
		anim		mCombat Idle TwoHand
		anim		mWalk OneHanded
		anim		mRun OneHanded
		anim		mStrafe Left OneHanded
		anim		mStrafe Right OneHanded
		anim		mWalk Combat
		anim		mRun Combat
		anim		mRide (Air)
		anim		mRide (Ground)
		anim		mMount
		anim		mDismount
		anim		mJump
		anim		mIn Air
		anim		mLanding
		anim		mLanding Small
		anim		mLanding Run
		anim		mNoncombat To Crouch
		anim		mCombat To Crouch
		anim		mNoncombat Crouched
		anim		mCombat Crouched
		anim		mWaddle Forward
		anim		mRoll Left
		anim		mRoll Right
		anim		mRoll Forward
		anim		mRoll Back
		anim		mCartwheel L
		anim		mCartwheel R
		anim		mFlip Forward
		anim		mFlip Back
		anim		mPush Rollable
		anim		mPush Slidable
		anim		mLadder Up 1
		anim		mLadder Up 2
		anim		mLadder Down 1
		anim		mLadder Down 2
		anim		mLadder Up L Idle
		anim		mLadder Up R Idle
		anim		mLadder Down R Idle
		anim		mGet On Ladder B
		anim		mGet Off Ladder B
		anim		mGet On Ladder T
		anim		mGet Off Ladder T
		anim		mStairs Up
		anim		mStairs Down
		anim		mStairs Up Idle L
		anim		mStairs Up Idle R
		anim		mStairs Down Idle L
		anim		mStairs Down Idle R
		anim		mHuh?
		anim		m180 Turn
		anim		mUse Magic Crystal
		anim		mSlide Forward
		anim		mSlide Backward
		anim		mBalancing
		anim		mThrow
		anim		mShoot Gun
		anim		mReload Gun
		anim		mArcher Medium
		anim		mArcher Low
		float		mGunCam Horz Distance
		float		mGunCam Vert Distance
		anim		mOneHand RH
		anim		mOneHand RM1
		anim		mOneHand RM2
		anim		mOneHand RM3
		anim		mOneHand RL
		anim		mOneHand SH
		anim		mOneHand SM
		anim		mOneHand SL
		anim		mOneHand UB1
		anim		mOneHand UB2
		anim		mOneHand UB3
		anim		mTwoHand RH
		anim		mTwoHand RM1
		anim		mTwoHand RM2
		anim		mTwoHand RM3
		anim		mTwoHand RL
		anim		mTwoHand SH
		anim		mTwoHand SM
		anim		mTwoHand SL
		anim		mTwoHand UB1
		anim		mTwoHand UB2
		anim		mTwoHand UB3
		anim		mFire SA
		anim		mIce SA
		anim		mLightning SA
		anim		mDeath SA
		anim		mStrafe Attack L UB
		anim		mStrafe Attack R UB
		anim		mRoll L Attack
		anim		mRoll R Attack
		anim		mSpecial Attack Thrust
		anim		mSpecial Attack Left
		anim		mSpecial Attack Right
		anim		mSpecial Attack Spin
		anim		mSpecial Attack Swing 180
		anim		mSpecial Attack Jump
		float		mSA Thrust Damage Multiplier
		float		mSA Left Damage Multiplier
		float		mSA Right Damage Multiplier
		float		mSA Spin Damage Multiplier
		float		mSA Swing 180 Damage Multiplier
		float		mSA Jump Damage Multiplier
		anim		mBlock
		anim		mCrouched Block
		int32		mBlock Damage Reduction %
		anim		mSheathe
		anim		mUnsheathe
		float		mRe-attack Window (s)
		anim		mHitRxn HSL
		anim		mHitRxn HSH
		anim		mHitRxn OSL
		anim		mHitRxn OSH
		anim		mHitRxn FSL
		anim		mHitRxn FSH
		sound[]		mHitRxn HSL
		sound[]		mHitRxn HSH
		sound[]		mHitRxn OSL
		sound[]		mHitRxn OSH
		sound[]		mHitRxn FSL
		sound[]		mHitRxn FSH
		anim		mWalk Strafe L
		anim		mWalk Strafe R
		anim		mSlow Walk
		anim		mRun Backwards
		anim		mRun Backwards One Handed
		anim		mNoncombat2Run
		anim		mRun2Ready
		anim		mRun2ReadyL
		anim		mWalk2Ready
		anim		mStrafeL2Ready
		anim		mStrafeR2Ready
		anim		mTurn Left
		anim		mTurn Right
		anim		mBump Into Wall
		float		mSwim Forward Speed
		float		mSwim Backward Speed
		float		mAir Consumption Rate (%/sec)
		float		mAir Replenishment Rate (%/sec)
		anim		mInAir To Swan Dive
		anim		mSwan Dive
		anim		mSwim
		anim		mBackswim
		anim		mSwim To Backswim
		anim		mBackswim To Swim
		anim		mTread Water
		anim		mIdle Underwater
		anim		mIdleUW to Tread
		anim		mSwim To Run
		sound		mSlide Sound (looped)
		sound		mHard Landing Sound
		sound[]		mDragon Call Sound
		sound[]		mHuh? Sound
		sound[]		mCan't Reach Sound
		sound[]		mCan't Use Sound
		sound		mFall Into Water Sound
		sound		mInventory Full Sound
		sound		mNo Darts Sound
		sound		mNo Arrows Sound
		sound		mSheathe Sound
		sound[]		mPush Object Sounds
		sound[]		mAttack Grunts
		sound[]		mGet-the-point Sounds
		int32		mGet the point Snd Chance (0-100)*/
    };
}

#endif /* RFLPLAYERCONTROL_H_ */
