/*
 * HumanControl.h
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_HUMANCONTROL_H_
#define INCLUDE_RFL_DRAGON_HUMANCONTROL_H_

#include <osg/NodeCallback>
#include <odCore/db/Animation.h>
#include <odCore/physics/CharacterController.h>
#include <odCore/Player.h>

#include <dragonRfl/classes/common/PlayerCommon.h>

namespace odAnim
{
    class SkeletonAnimationPlayer;
}

namespace dragonRfl
{
    class DragonRfl;

	/**
	 * Seperate container class for the RFL fields in HumanControl. This exists just so the actual
	 * RFL class HumanControl is not cluttered with the 179 members this class defines. Keeps logic and RFL stuff seperate.
	 */
	class HumanControlFields : public PlayerCommon
    {
    public:

		HumanControlFields();
        virtual ~HumanControlFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;


    protected:

        odRfl::RflCharChannel		mRightWeaponChannel;
		odRfl::RflCharChannel		mLeftWeaponChannel;
		odRfl::RflCharChannel		mTorsoChannel;
		odRfl::RflCharChannel		mLowerBodyChan;
		odRfl::RflCharChannel		mRightArmChan;
		odRfl::RflCharChannelArray	mSolveToChans;
		odRfl::RflCharChannelArray	mLeftLegChans;
		odRfl::RflCharChannelArray	mRightLegChans;
		odRfl::RflCharChannel		mHair1Channel;
		odRfl::RflFloat		        mMultiplayerSpeedScaling;
		odRfl::RflFloat		        mFallStartDmgSpeed;
		odRfl::RflFloat		        mFallDeathSpeed;
		odRfl::RflAnimRef		    mDrown;
		odRfl::RflSoundRef		    mDrownSound;
		odRfl::RflAnimRef		    mCombatIdleOneHand;
		odRfl::RflAnimRef		    mCombatIdleTwoHand;
		odRfl::RflAnimRef		    mWalkOneHanded;
		odRfl::RflAnimRef		    mRunOneHanded;
		odRfl::RflAnimRef		    mStrafeLeftOneHanded;
		odRfl::RflAnimRef		    mStrafeRightOneHanded;
		odRfl::RflAnimRef		    mWalkCombat;
		odRfl::RflAnimRef		    mRunCombat;
		odRfl::RflAnimRef		    mRideAir;
		odRfl::RflAnimRef		    mRideGround;
		odRfl::RflAnimRef		    mMount;
		odRfl::RflAnimRef		    mDismount;
		odRfl::RflAnimRef		    mJump;
		odRfl::RflAnimRef		    mInAir;
		odRfl::RflAnimRef		    mLanding;
		odRfl::RflAnimRef		    mLandingSmall;
		odRfl::RflAnimRef		    mLandingRun;
		odRfl::RflAnimRef		    mNoncombatToCrouch;
		odRfl::RflAnimRef		    mCombatToCrouch;
		odRfl::RflAnimRef		    mNoncombatCrouched;
		odRfl::RflAnimRef		    mCombatCrouched;
		odRfl::RflAnimRef		    mWaddleForward;
		odRfl::RflAnimRef		    mRollLeft;
		odRfl::RflAnimRef		    mRollRight;
		odRfl::RflAnimRef		    mRollForward;
		odRfl::RflAnimRef		    mRollBack;
		odRfl::RflAnimRef		    mCartwheelL;
		odRfl::RflAnimRef		    mCartwheelR;
		odRfl::RflAnimRef		    mFlipForward;
		odRfl::RflAnimRef		    mFlipBack;
		odRfl::RflAnimRef		    mPushRollable;
		odRfl::RflAnimRef		    mPushSlidable;
		odRfl::RflAnimRef		    mLadderUp1;
		odRfl::RflAnimRef		    mLadderUp2;
		odRfl::RflAnimRef		    mLadderDown1;
		odRfl::RflAnimRef		    mLadderDown2;
		odRfl::RflAnimRef		    mLadderUpLIdle;
		odRfl::RflAnimRef		    mLadderUpRIdle;
		odRfl::RflAnimRef		    mLadderDownRIdle;
		odRfl::RflAnimRef		    mGetOnLadderB;
		odRfl::RflAnimRef		    mGetOffLadderB;
		odRfl::RflAnimRef		    mGetOnLadderT;
		odRfl::RflAnimRef		    mGetOffLadderT;
		odRfl::RflAnimRef		    mStairsUp;
		odRfl::RflAnimRef		    mStairsDown;
		odRfl::RflAnimRef		    mStairsUpIdleL;
		odRfl::RflAnimRef		    mStairsUpIdleR;
		odRfl::RflAnimRef		    mStairsDownIdleL;
		odRfl::RflAnimRef		    mStairsDownIdleR;
		odRfl::RflAnimRef		    mHuh;
		odRfl::RflAnimRef		    m180Turn;
		odRfl::RflAnimRef		    mUseMagicCrystal;
		odRfl::RflAnimRef		    mSlideForward;
		odRfl::RflAnimRef		    mSlideBackward;
		odRfl::RflAnimRef		    mBalancing;
		odRfl::RflAnimRef		    mThrow;
		odRfl::RflAnimRef		    mShootGun;
		odRfl::RflAnimRef		    mReloadGun;
		odRfl::RflAnimRef		    mArcherMedium;
		odRfl::RflAnimRef		    mArcherLow;
		odRfl::RflFloat		        mGunCamHorzDistance;
		odRfl::RflFloat		        mGunCamVertDistance;
		odRfl::RflAnimRef		    mOneHandRH;
		odRfl::RflAnimRef		    mOneHandRM1;
		odRfl::RflAnimRef		    mOneHandRM2;
		odRfl::RflAnimRef		    mOneHandRM3;
		odRfl::RflAnimRef		    mOneHandRL;
		odRfl::RflAnimRef		    mOneHandSH;
		odRfl::RflAnimRef		    mOneHandSM;
		odRfl::RflAnimRef		    mOneHandSL;
		odRfl::RflAnimRef		    mOneHandUB1;
		odRfl::RflAnimRef		    mOneHandUB2;
		odRfl::RflAnimRef		    mOneHandUB3;
		odRfl::RflAnimRef		    mTwoHandRH;
		odRfl::RflAnimRef		    mTwoHandRM1;
		odRfl::RflAnimRef		    mTwoHandRM2;
		odRfl::RflAnimRef		    mTwoHandRM3;
		odRfl::RflAnimRef		    mTwoHandRL;
		odRfl::RflAnimRef		    mTwoHandSH;
		odRfl::RflAnimRef		    mTwoHandSM;
		odRfl::RflAnimRef		    mTwoHandSL;
		odRfl::RflAnimRef		    mTwoHandUB1;
		odRfl::RflAnimRef		    mTwoHandUB2;
		odRfl::RflAnimRef		    mTwoHandUB3;
		odRfl::RflAnimRef		    mFireSA;
		odRfl::RflAnimRef		    mIceSA;
		odRfl::RflAnimRef		    mLightningSA;
		odRfl::RflAnimRef		    mDeathSA;
		odRfl::RflAnimRef		    mStrafeAttackLUB;
		odRfl::RflAnimRef		    mStrafeAttackRUB;
		odRfl::RflAnimRef		    mRollLAttack;
		odRfl::RflAnimRef		    mRollRAttack;
		odRfl::RflAnimRef		    mSpecialAttackThrust;
		odRfl::RflAnimRef		    mSpecialAttackLeft;
		odRfl::RflAnimRef		    mSpecialAttackRight;
		odRfl::RflAnimRef		    mSpecialAttackSpin;
		odRfl::RflAnimRef		    mSpecialAttackSwing180;
		odRfl::RflAnimRef		    mSpecialAttackJump;
		odRfl::RflFloat		        mSAThrustDamageMultiplier;
		odRfl::RflFloat		        mSALeftDamageMultiplier;
		odRfl::RflFloat		        mSARightDamageMultiplier;
		odRfl::RflFloat		        mSASpinDamageMultiplier;
		odRfl::RflFloat		        mSASwing180DamageMultiplier;
		odRfl::RflFloat		        mSAJumpDamageMultiplier;
		odRfl::RflAnimRef		    mBlock;
		odRfl::RflAnimRef		    mCrouchedBlock;
		odRfl::RflInteger		    mBlockDamageReductionPercent;
		odRfl::RflAnimRef		    mSheathe;
		odRfl::RflAnimRef		    mUnsheathe;
		odRfl::RflFloat		        mReAttackWindow;
		odRfl::RflAnimRef		    mHitRxnHSLAnim;
		odRfl::RflAnimRef		    mHitRxnHSHAnim;
		odRfl::RflAnimRef		    mHitRxnOSLAnim;
		odRfl::RflAnimRef		    mHitRxnOSHAnim;
		odRfl::RflAnimRef		    mHitRxnFSLAnim;
		odRfl::RflAnimRef		    mHitRxnFSHAnim;
		odRfl::RflSoundRefArray	    mHitRxnHSLSound;
		odRfl::RflSoundRefArray	    mHitRxnHSHSound;
		odRfl::RflSoundRefArray	    mHitRxnOSLSound;
		odRfl::RflSoundRefArray	    mHitRxnOSHSound;
		odRfl::RflSoundRefArray	    mHitRxnFSLSound;
		odRfl::RflSoundRefArray	    mHitRxnFSHSound;
		odRfl::RflAnimRef		    mWalkStrafeL;
		odRfl::RflAnimRef		    mWalkStrafeR;
		odRfl::RflAnimRef		    mSlowWalk;
		odRfl::RflAnimRef		    mRunBackwards;
		odRfl::RflAnimRef		    mRunBackwardsOneHanded;
		odRfl::RflAnimRef		    mNoncombat2Run;
		odRfl::RflAnimRef		    mRun2Ready;
		odRfl::RflAnimRef		    mRun2ReadyL;
		odRfl::RflAnimRef		    mWalk2Ready;
		odRfl::RflAnimRef		    mStrafeL2Ready;
		odRfl::RflAnimRef		    mStrafeR2Ready;
		odRfl::RflAnimRef		    mTurnLeft;
		odRfl::RflAnimRef		    mTurnRight;
		odRfl::RflAnimRef		    mBumpIntoWall;
		odRfl::RflFloat		        mSwimForwardSpeed;
		odRfl::RflFloat		        mSwimBackwardSpeed;
		odRfl::RflFloat		        mAirConsumptionRate;
		odRfl::RflFloat		        mAirReplenishmentRate;
		odRfl::RflAnimRef		    mInAirToSwanDive;
		odRfl::RflAnimRef		    mSwanDive;
		odRfl::RflAnimRef		    mSwim;
		odRfl::RflAnimRef		    mBackswim;
		odRfl::RflAnimRef		    mSwimToBackswim;
		odRfl::RflAnimRef		    mBackswimToSwim;
		odRfl::RflAnimRef		    mTreadWater;
		odRfl::RflAnimRef		    mIdleUnderwater;
		odRfl::RflAnimRef		    mIdleUWtoTread;
		odRfl::RflAnimRef		    mSwimToRun;
		odRfl::RflSoundRef		    mSlideSound;
		odRfl::RflSoundRef		    mHardLandingSound;
		odRfl::RflSoundRefArray	    mDragonCallSounds;
		odRfl::RflSoundRefArray	    mHuhSounds;
		odRfl::RflSoundRefArray	    mCantReachSounds;
		odRfl::RflSoundRefArray	    mCantUseSounds;
		odRfl::RflSoundRef			mFallIntoWaterSound;
		odRfl::RflSoundRef			mInventoryFullSound;
		odRfl::RflSoundRef			mNoDartsSound;
		odRfl::RflSoundRef			mNoArrowsSound;
		odRfl::RflSoundRef			mSheatheSound;
		odRfl::RflSoundRefArray 	mPushObjectSounds;
		odRfl::RflSoundRefArray	    mAttackGrunts;
		odRfl::RflSoundRefArray	    mGetThePointSounds;
		odRfl::RflInteger			mGetThePointSndChance;
    };



	class HumanControl : public HumanControlFields, public od::Player
	{
	public:

		HumanControl(DragonRfl &rfl);
		virtual ~HumanControl();

		virtual void onLoaded(od::LevelObject &obj) override;
		virtual void onSpawned(od::LevelObject &obj) override;
		virtual void onUpdate(od::LevelObject &obj, double simTime, double relTime) override;
		virtual void onMoved(od::LevelObject &obj) override;


		// implement od::Player
		virtual float getYaw() const override { return mYaw; }
		virtual void setYaw(float f) override { mYaw = f; }
		virtual float getPitch() const override { return mPitch; }
		virtual void setPitch(float f) override { mPitch = f; }
		virtual void moveForward(float speed) override;
		virtual void moveRight(float speed) override;
		virtual osg::Vec3f getPosition() override;
		virtual od::LevelObject &getLevelObject() override;


	 private:

		void _updateMotion(double relTime);

		float mYaw;
		float mPitch;
		float mPrevYaw;
		float mForwardSpeed;
		float mRightSpeed;
		osg::ref_ptr<osg::NodeCallback> mUpdateCallback;
		od::LevelObject *mPlayerObject;
		osg::ref_ptr<odAnim::SkeletonAnimationPlayer> mAnimationPlayer;
		std::unique_ptr<odPhysics::CharacterController> mCharacterController;

	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0009, "Player", "Human Control", dragonRfl::HumanControl);

#endif /* INCLUDE_RFL_DRAGON_HUMANCONTROL_H_ */
