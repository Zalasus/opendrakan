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

        odRfl::CharChannel		mRightWeaponChannel;
		odRfl::CharChannel		mLeftWeaponChannel;
		odRfl::CharChannel		mTorsoChannel;
		odRfl::CharChannel		mLowerBodyChan;
		odRfl::CharChannel		mRightArmChan;
		odRfl::CharChannelArray	mSolveToChans;
		odRfl::CharChannelArray	mLeftLegChans;
		odRfl::CharChannelArray	mRightLegChans;
		odRfl::CharChannel		mHair1Channel;
		odRfl::Float		    mMultiplayerSpeedScaling;
		odRfl::Float		    mFallStartDmgSpeed;
		odRfl::Float		    mFallDeathSpeed;
		odRfl::AnimRef		    mDrown;
		odRfl::SoundRef		    mDrownSound;
		odRfl::AnimRef		    mCombatIdleOneHand;
		odRfl::AnimRef		    mCombatIdleTwoHand;
		odRfl::AnimRef		    mWalkOneHanded;
		odRfl::AnimRef		    mRunOneHanded;
		odRfl::AnimRef		    mStrafeLeftOneHanded;
		odRfl::AnimRef		    mStrafeRightOneHanded;
		odRfl::AnimRef		    mWalkCombat;
		odRfl::AnimRef		    mRunCombat;
		odRfl::AnimRef		    mRideAir;
		odRfl::AnimRef		    mRideGround;
		odRfl::AnimRef		    mMount;
		odRfl::AnimRef		    mDismount;
		odRfl::AnimRef		    mJump;
		odRfl::AnimRef		    mInAir;
		odRfl::AnimRef		    mLanding;
		odRfl::AnimRef		    mLandingSmall;
		odRfl::AnimRef		    mLandingRun;
		odRfl::AnimRef		    mNoncombatToCrouch;
		odRfl::AnimRef		    mCombatToCrouch;
		odRfl::AnimRef		    mNoncombatCrouched;
		odRfl::AnimRef		    mCombatCrouched;
		odRfl::AnimRef		    mWaddleForward;
		odRfl::AnimRef		    mRollLeft;
		odRfl::AnimRef		    mRollRight;
		odRfl::AnimRef		    mRollForward;
		odRfl::AnimRef		    mRollBack;
		odRfl::AnimRef		    mCartwheelL;
		odRfl::AnimRef		    mCartwheelR;
		odRfl::AnimRef		    mFlipForward;
		odRfl::AnimRef		    mFlipBack;
		odRfl::AnimRef		    mPushRollable;
		odRfl::AnimRef		    mPushSlidable;
		odRfl::AnimRef		    mLadderUp1;
		odRfl::AnimRef		    mLadderUp2;
		odRfl::AnimRef		    mLadderDown1;
		odRfl::AnimRef		    mLadderDown2;
		odRfl::AnimRef		    mLadderUpLIdle;
		odRfl::AnimRef		    mLadderUpRIdle;
		odRfl::AnimRef		    mLadderDownRIdle;
		odRfl::AnimRef		    mGetOnLadderB;
		odRfl::AnimRef		    mGetOffLadderB;
		odRfl::AnimRef		    mGetOnLadderT;
		odRfl::AnimRef		    mGetOffLadderT;
		odRfl::AnimRef		    mStairsUp;
		odRfl::AnimRef		    mStairsDown;
		odRfl::AnimRef		    mStairsUpIdleL;
		odRfl::AnimRef		    mStairsUpIdleR;
		odRfl::AnimRef		    mStairsDownIdleL;
		odRfl::AnimRef		    mStairsDownIdleR;
		odRfl::AnimRef		    mHuh;
		odRfl::AnimRef		    m180Turn;
		odRfl::AnimRef		    mUseMagicCrystal;
		odRfl::AnimRef		    mSlideForward;
		odRfl::AnimRef		    mSlideBackward;
		odRfl::AnimRef		    mBalancing;
		odRfl::AnimRef		    mThrow;
		odRfl::AnimRef		    mShootGun;
		odRfl::AnimRef		    mReloadGun;
		odRfl::AnimRef		    mArcherMedium;
		odRfl::AnimRef		    mArcherLow;
		odRfl::Float		    mGunCamHorzDistance;
		odRfl::Float		    mGunCamVertDistance;
		odRfl::AnimRef		    mOneHandRH;
		odRfl::AnimRef		    mOneHandRM1;
		odRfl::AnimRef		    mOneHandRM2;
		odRfl::AnimRef		    mOneHandRM3;
		odRfl::AnimRef		    mOneHandRL;
		odRfl::AnimRef		    mOneHandSH;
		odRfl::AnimRef		    mOneHandSM;
		odRfl::AnimRef		    mOneHandSL;
		odRfl::AnimRef		    mOneHandUB1;
		odRfl::AnimRef		    mOneHandUB2;
		odRfl::AnimRef		    mOneHandUB3;
		odRfl::AnimRef		    mTwoHandRH;
		odRfl::AnimRef		    mTwoHandRM1;
		odRfl::AnimRef		    mTwoHandRM2;
		odRfl::AnimRef		    mTwoHandRM3;
		odRfl::AnimRef		    mTwoHandRL;
		odRfl::AnimRef		    mTwoHandSH;
		odRfl::AnimRef		    mTwoHandSM;
		odRfl::AnimRef		    mTwoHandSL;
		odRfl::AnimRef		    mTwoHandUB1;
		odRfl::AnimRef		    mTwoHandUB2;
		odRfl::AnimRef		    mTwoHandUB3;
		odRfl::AnimRef		    mFireSA;
		odRfl::AnimRef		    mIceSA;
		odRfl::AnimRef		    mLightningSA;
		odRfl::AnimRef		    mDeathSA;
		odRfl::AnimRef		    mStrafeAttackLUB;
		odRfl::AnimRef		    mStrafeAttackRUB;
		odRfl::AnimRef		    mRollLAttack;
		odRfl::AnimRef		    mRollRAttack;
		odRfl::AnimRef		    mSpecialAttackThrust;
		odRfl::AnimRef		    mSpecialAttackLeft;
		odRfl::AnimRef		    mSpecialAttackRight;
		odRfl::AnimRef		    mSpecialAttackSpin;
		odRfl::AnimRef		    mSpecialAttackSwing180;
		odRfl::AnimRef		    mSpecialAttackJump;
		odRfl::Float		    mSAThrustDamageMultiplier;
		odRfl::Float		    mSALeftDamageMultiplier;
		odRfl::Float		    mSARightDamageMultiplier;
		odRfl::Float		    mSASpinDamageMultiplier;
		odRfl::Float		    mSASwing180DamageMultiplier;
		odRfl::Float		    mSAJumpDamageMultiplier;
		odRfl::AnimRef		    mBlock;
		odRfl::AnimRef		    mCrouchedBlock;
		odRfl::Integer		    mBlockDamageReductionPercent;
		odRfl::AnimRef		    mSheathe;
		odRfl::AnimRef		    mUnsheathe;
		odRfl::Float		    mReAttackWindow;
		odRfl::AnimRef		    mHitRxnHSLAnim;
		odRfl::AnimRef		    mHitRxnHSHAnim;
		odRfl::AnimRef		    mHitRxnOSLAnim;
		odRfl::AnimRef		    mHitRxnOSHAnim;
		odRfl::AnimRef		    mHitRxnFSLAnim;
		odRfl::AnimRef		    mHitRxnFSHAnim;
		odRfl::SoundRefArray	mHitRxnHSLSound;
		odRfl::SoundRefArray	mHitRxnHSHSound;
		odRfl::SoundRefArray	mHitRxnOSLSound;
		odRfl::SoundRefArray	mHitRxnOSHSound;
		odRfl::SoundRefArray	mHitRxnFSLSound;
		odRfl::SoundRefArray	mHitRxnFSHSound;
		odRfl::AnimRef		    mWalkStrafeL;
		odRfl::AnimRef		    mWalkStrafeR;
		odRfl::AnimRef		    mSlowWalk;
		odRfl::AnimRef		    mRunBackwards;
		odRfl::AnimRef		    mRunBackwardsOneHanded;
		odRfl::AnimRef		    mNoncombat2Run;
		odRfl::AnimRef		    mRun2Ready;
		odRfl::AnimRef		    mRun2ReadyL;
		odRfl::AnimRef		    mWalk2Ready;
		odRfl::AnimRef		    mStrafeL2Ready;
		odRfl::AnimRef		    mStrafeR2Ready;
		odRfl::AnimRef		    mTurnLeft;
		odRfl::AnimRef		    mTurnRight;
		odRfl::AnimRef		    mBumpIntoWall;
		odRfl::Float		    mSwimForwardSpeed;
		odRfl::Float		    mSwimBackwardSpeed;
		odRfl::Float		    mAirConsumptionRate;
		odRfl::Float		    mAirReplenishmentRate;
		odRfl::AnimRef		    mInAirToSwanDive;
		odRfl::AnimRef		    mSwanDive;
		odRfl::AnimRef		    mSwim;
		odRfl::AnimRef		    mBackswim;
		odRfl::AnimRef		    mSwimToBackswim;
		odRfl::AnimRef		    mBackswimToSwim;
		odRfl::AnimRef		    mTreadWater;
		odRfl::AnimRef		    mIdleUnderwater;
		odRfl::AnimRef		    mIdleUWtoTread;
		odRfl::AnimRef		    mSwimToRun;
		odRfl::SoundRef		    mSlideSound;
		odRfl::SoundRef		    mHardLandingSound;
		odRfl::SoundRefArray	mDragonCallSounds;
		odRfl::SoundRefArray	mHuhSounds;
		odRfl::SoundRefArray	mCantReachSounds;
		odRfl::SoundRefArray	mCantUseSounds;
		odRfl::SoundRef			mFallIntoWaterSound;
		odRfl::SoundRef			mInventoryFullSound;
		odRfl::SoundRef			mNoDartsSound;
		odRfl::SoundRef			mNoArrowsSound;
		odRfl::SoundRef			mSheatheSound;
		odRfl::SoundRefArray 	mPushObjectSounds;
		odRfl::SoundRefArray	mAttackGrunts;
		odRfl::SoundRefArray	mGetThePointSounds;
		odRfl::Integer			mGetThePointSndChance;
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
