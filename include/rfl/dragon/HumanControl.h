/*
 * HumanControl.h
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_HUMANCONTROL_H_
#define INCLUDE_RFL_DRAGON_HUMANCONTROL_H_

#include "rfl/common/PlayerCommon.h"

#include <osg/NodeCallback>

#include "db/Animation.h"
#include "Player.h"

namespace odRfl
{

	class HumanControl : public PlayerCommon, public od::Player
    {
    public:

        HumanControl();
        virtual ~HumanControl();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawn(od::LevelObject &obj) override;

        // implement od::Player
    	virtual float getYaw() const override { return mYaw; }
    	virtual void setYaw(float f) override { mYaw = f; }
    	virtual float getPitch() const override { return mPitch; }
    	virtual void setPitch(float f) override { mPitch = f; }
    	virtual void moveForward(float speed) override;
    	virtual void moveRight(float speed) override;
    	virtual osg::Vec3f getPosition() override;

    	void update(double simTime, double relTime);


    protected:

        // 179 members, woo! TODO: maybe put them in an array or something? would need to extend our reflection concept a bit
        RflCharChannel		mRightWeaponChannel;
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
		RflInteger			mGetThePointSndChance;


    private:

    	void _updateMotion(double relTime);

    	float mYaw;
    	float mPitch;
    	float mPrevYaw;
    	float mForwardSpeed;
    	float mRightSpeed;
    	osg::ref_ptr<osg::NodeCallback> mUpdateCallback;
    	osg::ref_ptr<od::LevelObject> mPlayerObject;
    };

}

#endif /* INCLUDE_RFL_DRAGON_HUMANCONTROL_H_ */
