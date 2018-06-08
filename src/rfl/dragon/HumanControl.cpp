/*
 * HumanControl.cpp
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#include "rfl/dragon/HumanControl.h"

#include "rfl/Rfl.h"
#include "LevelObject.h"
#include "Level.h"
#include "Player.h"
#include "Engine.h"
#include "Camera.h"
#include "anim/SkeletonAnimationPlayer.h"

namespace odRfl
{

	HumanControlFields::HumanControlFields()
	: mRightWeaponChannel(0)
	, mLeftWeaponChannel(0)
	, mTorsoChannel(0)
	, mLowerBodyChan(0)
	, mRightArmChan(0)
	, mSolveToChans({})
	, mLeftLegChans({})
	, mRightLegChans({})
	, mHair1Channel(0)
	, mMultiplayerSpeedScaling(1.0)
	, mFallStartDmgSpeed(4.0)
	, mFallDeathSpeed(7.0)
	, mDrown(od::AssetRef::NULL_REF)
	, mDrownSound(od::AssetRef::NULL_REF)
	, mCombatIdleOneHand(od::AssetRef::NULL_REF)
	, mCombatIdleTwoHand(od::AssetRef::NULL_REF)
	, mWalkOneHanded(od::AssetRef::NULL_REF)
	, mRunOneHanded(od::AssetRef::NULL_REF)
	, mStrafeLeftOneHanded(od::AssetRef::NULL_REF)
	, mStrafeRightOneHanded(od::AssetRef::NULL_REF)
	, mWalkCombat(od::AssetRef::NULL_REF)
	, mRunCombat(od::AssetRef::NULL_REF)
	, mRideAir(od::AssetRef::NULL_REF)
	, mRideGround(od::AssetRef::NULL_REF)
	, mMount(od::AssetRef::NULL_REF)
	, mDismount(od::AssetRef::NULL_REF)
	, mJump(od::AssetRef::NULL_REF)
	, mInAir(od::AssetRef::NULL_REF)
	, mLanding(od::AssetRef::NULL_REF)
	, mLandingSmall(od::AssetRef::NULL_REF)
	, mLandingRun(od::AssetRef::NULL_REF)
	, mNoncombatToCrouch(od::AssetRef::NULL_REF)
	, mCombatToCrouch(od::AssetRef::NULL_REF)
	, mNoncombatCrouched(od::AssetRef::NULL_REF)
	, mCombatCrouched(od::AssetRef::NULL_REF)
	, mWaddleForward(od::AssetRef::NULL_REF)
	, mRollLeft(od::AssetRef::NULL_REF)
	, mRollRight(od::AssetRef::NULL_REF)
	, mRollForward(od::AssetRef::NULL_REF)
	, mRollBack(od::AssetRef::NULL_REF)
	, mCartwheelL(od::AssetRef::NULL_REF)
	, mCartwheelR(od::AssetRef::NULL_REF)
	, mFlipForward(od::AssetRef::NULL_REF)
	, mFlipBack(od::AssetRef::NULL_REF)
	, mPushRollable(od::AssetRef::NULL_REF)
	, mPushSlidable(od::AssetRef::NULL_REF)
	, mLadderUp1(od::AssetRef::NULL_REF)
	, mLadderUp2(od::AssetRef::NULL_REF)
	, mLadderDown1(od::AssetRef::NULL_REF)
	, mLadderDown2(od::AssetRef::NULL_REF)
	, mLadderUpLIdle(od::AssetRef::NULL_REF)
	, mLadderUpRIdle(od::AssetRef::NULL_REF)
	, mLadderDownRIdle(od::AssetRef::NULL_REF)
	, mGetOnLadderB(od::AssetRef::NULL_REF)
	, mGetOffLadderB(od::AssetRef::NULL_REF)
	, mGetOnLadderT(od::AssetRef::NULL_REF)
	, mGetOffLadderT(od::AssetRef::NULL_REF)
	, mStairsUp(od::AssetRef::NULL_REF)
	, mStairsDown(od::AssetRef::NULL_REF)
	, mStairsUpIdleL(od::AssetRef::NULL_REF)
	, mStairsUpIdleR(od::AssetRef::NULL_REF)
	, mStairsDownIdleL(od::AssetRef::NULL_REF)
	, mStairsDownIdleR(od::AssetRef::NULL_REF)
	, mHuh(od::AssetRef::NULL_REF)
	, m180Turn(od::AssetRef::NULL_REF)
	, mUseMagicCrystal(od::AssetRef::NULL_REF)
	, mSlideForward(od::AssetRef::NULL_REF)
	, mSlideBackward(od::AssetRef::NULL_REF)
	, mBalancing(od::AssetRef::NULL_REF)
	, mThrow(od::AssetRef::NULL_REF)
	, mShootGun(od::AssetRef::NULL_REF)
	, mReloadGun(od::AssetRef::NULL_REF)
	, mArcherMedium(od::AssetRef::NULL_REF)
	, mArcherLow(od::AssetRef::NULL_REF)
	, mGunCamHorzDistance(1024.0)
	, mGunCamVertDistance(1024.0)
	, mOneHandRH(od::AssetRef::NULL_REF)
	, mOneHandRM1(od::AssetRef::NULL_REF)
	, mOneHandRM2(od::AssetRef::NULL_REF)
	, mOneHandRM3(od::AssetRef::NULL_REF)
	, mOneHandRL(od::AssetRef::NULL_REF)
	, mOneHandSH(od::AssetRef::NULL_REF)
	, mOneHandSM(od::AssetRef::NULL_REF)
	, mOneHandSL(od::AssetRef::NULL_REF)
	, mOneHandUB1(od::AssetRef::NULL_REF)
	, mOneHandUB2(od::AssetRef::NULL_REF)
	, mOneHandUB3(od::AssetRef::NULL_REF)
	, mTwoHandRH(od::AssetRef::NULL_REF)
	, mTwoHandRM1(od::AssetRef::NULL_REF)
	, mTwoHandRM2(od::AssetRef::NULL_REF)
	, mTwoHandRM3(od::AssetRef::NULL_REF)
	, mTwoHandRL(od::AssetRef::NULL_REF)
	, mTwoHandSH(od::AssetRef::NULL_REF)
	, mTwoHandSM(od::AssetRef::NULL_REF)
	, mTwoHandSL(od::AssetRef::NULL_REF)
	, mTwoHandUB1(od::AssetRef::NULL_REF)
	, mTwoHandUB2(od::AssetRef::NULL_REF)
	, mTwoHandUB3(od::AssetRef::NULL_REF)
	, mFireSA(od::AssetRef::NULL_REF)
	, mIceSA(od::AssetRef::NULL_REF)
	, mLightningSA(od::AssetRef::NULL_REF)
	, mDeathSA(od::AssetRef::NULL_REF)
	, mStrafeAttackLUB(od::AssetRef::NULL_REF)
	, mStrafeAttackRUB(od::AssetRef::NULL_REF)
	, mRollLAttack(od::AssetRef::NULL_REF)
	, mRollRAttack(od::AssetRef::NULL_REF)
	, mSpecialAttackThrust(od::AssetRef::NULL_REF)
	, mSpecialAttackLeft(od::AssetRef::NULL_REF)
	, mSpecialAttackRight(od::AssetRef::NULL_REF)
	, mSpecialAttackSpin(od::AssetRef::NULL_REF)
	, mSpecialAttackSwing180(od::AssetRef::NULL_REF)
	, mSpecialAttackJump(od::AssetRef::NULL_REF)
	, mSAThrustDamageMultiplier(1.25)
	, mSALeftDamageMultiplier(1.25)
	, mSARightDamageMultiplier(1.25)
	, mSASpinDamageMultiplier(1.25)
	, mSASwing180DamageMultiplier(1.25)
	, mSAJumpDamageMultiplier(1.25)
	, mBlock(od::AssetRef::NULL_REF)
	, mCrouchedBlock(od::AssetRef::NULL_REF)
	, mBlockDamageReductionPercent(25)
	, mSheathe(od::AssetRef::NULL_REF)
	, mUnsheathe(od::AssetRef::NULL_REF)
	, mReAttackWindow(0.2)
	, mHitRxnHSLAnim(od::AssetRef::NULL_REF)
	, mHitRxnHSHAnim(od::AssetRef::NULL_REF)
	, mHitRxnOSLAnim(od::AssetRef::NULL_REF)
	, mHitRxnOSHAnim(od::AssetRef::NULL_REF)
	, mHitRxnFSLAnim(od::AssetRef::NULL_REF)
	, mHitRxnFSHAnim(od::AssetRef::NULL_REF)
	, mHitRxnHSLSound({})
	, mHitRxnHSHSound({})
	, mHitRxnOSLSound({})
	, mHitRxnOSHSound({})
	, mHitRxnFSLSound({})
	, mHitRxnFSHSound({})
	, mWalkStrafeL(od::AssetRef::NULL_REF)
	, mWalkStrafeR(od::AssetRef::NULL_REF)
	, mSlowWalk(od::AssetRef::NULL_REF)
	, mRunBackwards(od::AssetRef::NULL_REF)
	, mRunBackwardsOneHanded(od::AssetRef::NULL_REF)
	, mNoncombat2Run(od::AssetRef::NULL_REF)
	, mRun2Ready(od::AssetRef::NULL_REF)
	, mRun2ReadyL(od::AssetRef::NULL_REF)
	, mWalk2Ready(od::AssetRef::NULL_REF)
	, mStrafeL2Ready(od::AssetRef::NULL_REF)
	, mStrafeR2Ready(od::AssetRef::NULL_REF)
	, mTurnLeft(od::AssetRef::NULL_REF)
	, mTurnRight(od::AssetRef::NULL_REF)
	, mBumpIntoWall(od::AssetRef::NULL_REF)
	, mSwimForwardSpeed(1.2)
	, mSwimBackwardSpeed(-0.7)
	, mAirConsumptionRate(2.0)
	, mAirReplenishmentRate(2.0)
	, mInAirToSwanDive(od::AssetRef::NULL_REF)
	, mSwanDive(od::AssetRef::NULL_REF)
	, mSwim(od::AssetRef::NULL_REF)
	, mBackswim(od::AssetRef::NULL_REF)
	, mSwimToBackswim(od::AssetRef::NULL_REF)
	, mBackswimToSwim(od::AssetRef::NULL_REF)
	, mTreadWater(od::AssetRef::NULL_REF)
	, mIdleUnderwater(od::AssetRef::NULL_REF)
	, mIdleUWtoTread(od::AssetRef::NULL_REF)
	, mSwimToRun(od::AssetRef::NULL_REF)
	, mSlideSound(od::AssetRef::NULL_REF)
	, mHardLandingSound(od::AssetRef::NULL_REF)
	, mDragonCallSounds({})
	, mHuhSounds({})
	, mCantReachSounds({})
	, mCantUseSounds({})
	, mFallIntoWaterSound(od::AssetRef::NULL_REF)
	, mInventoryFullSound(od::AssetRef::NULL_REF)
	, mNoDartsSound(od::AssetRef::NULL_REF)
	, mNoArrowsSound(od::AssetRef::NULL_REF)
	, mSheatheSound(od::AssetRef::NULL_REF)
	, mPushObjectSounds({})
	, mAttackGrunts({})
	, mGetThePointSounds({})
	, mGetThePointSndChance(50)
    {
    }

	HumanControlFields::~HumanControlFields()
	{
	}

    void HumanControlFields::probeFields(RflFieldProbe &probe)
    {
    	PlayerCommon::probeFields(probe);

    	probe
			("Character Channels")
			   (mRightWeaponChannel, "Right Weapon Channel")
			   (mLeftWeaponChannel, "Left Weapon Channel")
			   (mTorsoChannel, "Torso chan")
			   (mLowerBodyChan, "Lower Body chan")
			   (mRightArmChan, "Right Arm chan")
			   (mSolveToChans, "Solve To Chans")
			   (mLeftLegChans, "Left Leg Chans")
			   (mRightLegChans, "Right Leg Chans")
			   (mHair1Channel, "Hair1 Channel")
		   ("General Motion Speeds")
			   (mMultiplayerSpeedScaling, "Multiplayer Speed Scaling")
		   ("Health And Damage")
			   (mFallStartDmgSpeed, "Fall Start Dmg Speed (lu/s)")
			   (mFallDeathSpeed, "Fall Death Speed (lu/s)")
		   ("Death")
			   (mDrown, "Drown")
			   (mDrownSound, "Drown Sound")
		   ("Movement With Weapons Animations")
			   (mCombatIdleOneHand, "Combat Idle OneHand")
			   (mCombatIdleTwoHand, "Combat Idle TwoHand")
			   (mWalkOneHanded, "Walk OneHanded")
			   (mRunOneHanded, "Run OneHanded")
			   (mStrafeLeftOneHanded, "Strafe Left OneHanded")
			   (mStrafeRightOneHanded, "Strafe Right OneHanded")
			   (mWalkCombat, "Walk Combat")
			   (mRunCombat, "Run Combat")
		   ("Dragon Interaction")
			   (mRideAir, "Ride (Air)")
			   (mRideGround, "Ride (Ground)")
			   (mMount, "Mount")
			   (mDismount, "Dismount")
		   ("Jump, Fall, And Land Animations")
			   (mJump, "Jump")
			   (mInAir, "In Air")
			   (mLanding, "Landing")
			   (mLandingSmall, "Landing Small")
			   (mLandingRun, "Landing Run")
		   ("Crouch Animations")
			   (mNoncombatToCrouch, "Noncombat To Crouch")
			   (mCombatToCrouch, "Combat To Crouch")
			   (mNoncombatCrouched, "Noncombat Crouched")
			   (mCombatCrouched, "Combat Crouched")
			   (mWaddleForward, "Waddle Forward")
			   (mRollLeft, "Roll Left")
			   (mRollRight, "Roll Right")
			   (mRollForward, "Roll Forward")
			   (mRollBack, "Roll Back")
		   ("Special Moves Animations")
			   (mCartwheelL, "Cartwheel L")
			   (mCartwheelR, "Cartwheel R")
			   (mFlipForward, "Flip Forward")
			   (mFlipBack, "Flip Back")
		   ("Object Interaction Animations")
			   (mPushRollable, "Push Rollable")
			   (mPushSlidable, "Push Slidable")
			   (mLadderUp1, "Ladder Up 1")
			   (mLadderUp2, "Ladder Up 2")
			   (mLadderDown1, "Ladder Down 1")
			   (mLadderDown2, "Ladder Down 2")
			   (mLadderUpLIdle, "Ladder Up L Idle")
			   (mLadderUpRIdle, "Ladder Up R Idle")
			   (mLadderDownRIdle, "Ladder Down R Idle")
			   (mGetOnLadderB, "Get On Ladder B")
			   (mGetOffLadderB, "Get Off Ladder B")
			   (mGetOnLadderT, "Get On Ladder T")
			   (mGetOffLadderT, "Get Off Ladder T")
			   (mStairsUp, "Stairs Up")
			   (mStairsDown, "Stairs Down")
			   (mStairsUpIdleL, "Stairs Up Idle L")
			   (mStairsUpIdleR, "Stairs Up Idle R")
			   (mStairsDownIdleL, "Stairs Down Idle L")
			   (mStairsDownIdleR, "Stairs Down Idle R")
			   (mHuh, "Huh?")
			   (m180Turn, "180 Turn")
			   (mUseMagicCrystal, "Use Magic Crystal")
		   ("Environment Interaction Animations")
			   (mSlideForward, "Slide Forward")
			   (mSlideBackward, "Slide Backward")
			   (mBalancing, "Balancing")
		   ("Projectile Weapons")
			   (mThrow, "Throw")
			   (mShootGun, "Shoot Gun")
			   (mReloadGun, "Reload Gun")
			   (mArcherMedium, "Archer Medium")
			   (mArcherLow, "Archer Low")
			   (mGunCamHorzDistance, "GunCam Horz Distance")
			   (mGunCamVertDistance, "GunCam Vert Distance")
		   ("One Handed Attack Animations")
			   (mOneHandRH, "OneHand RH")
			   (mOneHandRM1, "OneHand RM1")
			   (mOneHandRM2, "OneHand RM2")
			   (mOneHandRM3, "OneHand RM3")
			   (mOneHandRL, "OneHand RL")
			   (mOneHandSH, "OneHand SH")
			   (mOneHandSM, "OneHand SM")
			   (mOneHandSL, "OneHand SL")
			   (mOneHandUB1, "OneHand UB1")
			   (mOneHandUB2, "OneHand UB2")
			   (mOneHandUB3, "OneHand UB3")
		   ("Two Handed Attack Animations")
			   (mTwoHandRH, "TwoHand RH")
			   (mTwoHandRM1, "TwoHand RM1")
			   (mTwoHandRM2, "TwoHand RM2")
			   (mTwoHandRM3, "TwoHand RM3")
			   (mTwoHandRL, "TwoHand RL")
			   (mTwoHandSH, "TwoHand SH")
			   (mTwoHandSM, "TwoHand SM")
			   (mTwoHandSL, "TwoHand SL")
			   (mTwoHandUB1, "TwoHand UB1")
			   (mTwoHandUB2, "TwoHand UB2")
			   (mTwoHandUB3, "TwoHand UB3")
		   ("Special Magic Attack Animations")
			   (mFireSA, "Fire SA")
			   (mIceSA, "Ice SA")
			   (mLightningSA, "Lightning SA")
			   (mDeathSA, "Death SA")
		   ("Combo Attack Animations")
			   (mStrafeAttackLUB, "Strafe Attack L UB")
			   (mStrafeAttackRUB, "Strafe Attack R UB")
			   (mRollLAttack, "Roll L Attack")
			   (mRollRAttack, "Roll R Attack")
			   (mSpecialAttackThrust, "Special Attack Thrust")
			   (mSpecialAttackLeft, "Special Attack Left")
			   (mSpecialAttackRight, "Special Attack Right")
			   (mSpecialAttackSpin, "Special Attack Spin")
			   (mSpecialAttackSwing180, "Special Attack Swing 180")
			   (mSpecialAttackJump, "Special Attack Jump")
		   ("Special Attack Damage")
			   (mSAThrustDamageMultiplier, "SA Thrust Damage Multiplier")
			   (mSALeftDamageMultiplier, "SA Left Damage Multiplier")
			   (mSARightDamageMultiplier, "SA Right Damage Multiplier")
			   (mSASpinDamageMultiplier, "SA Spin Damage Multiplier")
			   (mSASwing180DamageMultiplier, "SA Swing 180 Damage Multiplier")
			   (mSAJumpDamageMultiplier, "SA Jump Damage Multiplier")
		   ("Defense")
			   (mBlock, "Block")
			   (mCrouchedBlock, "Crouched Block")
			   (mBlockDamageReductionPercent, "Block Damage Reduction %")
		   ("Sheathe and Unsheathe")
			   (mSheathe, "Sheathe")
			   (mUnsheathe, "Unsheathe")
		   ("Attack Factors")
			   (mReAttackWindow, "Re-attack Window (s)")
		   ("Damage Reaction Animations") // prime example that field names are not unique
			   (mHitRxnHSLAnim, "HitRxn HSL")
			   (mHitRxnHSHAnim, "HitRxn HSH")
			   (mHitRxnOSLAnim, "HitRxn OSL")
			   (mHitRxnOSHAnim, "HitRxn OSH")
			   (mHitRxnFSLAnim, "HitRxn FSL")
			   (mHitRxnFSHAnim, "HitRxn FSH")
		   ("Damage Reaction Sounds")
			   (mHitRxnHSLSound, "HitRxn HSL")
			   (mHitRxnHSHSound, "HitRxn HSH")
			   (mHitRxnOSLSound, "HitRxn OSL")
			   (mHitRxnOSHSound, "HitRxn OSH")
			   (mHitRxnFSLSound, "HitRxn FSL")
			   (mHitRxnFSHSound, "HitRxn FSH")
		   ("Ground Motion Animations")
			   (mWalkStrafeL, "Walk Strafe L")
			   (mWalkStrafeR, "Walk Strafe R")
			   (mSlowWalk, "Slow Walk")
			   (mRunBackwards, "Run Backwards")
			   (mRunBackwardsOneHanded, "Run Backwards One Handed")
			   (mNoncombat2Run, "Noncombat2Run")
			   (mRun2Ready, "Run2Ready")
			   (mRun2ReadyL, "Run2ReadyL")
			   (mWalk2Ready, "Walk2Ready")
			   (mStrafeL2Ready, "StrafeL2Ready")
			   (mStrafeR2Ready, "StrafeR2Ready")
			   (mTurnLeft, "Turn Left")
			   (mTurnRight, "Turn Right")
			   (mBumpIntoWall, "Bump Into Wall")
		   ("Swimming and Water Motion")
			   (mSwimForwardSpeed, "Swim Forward Speed")
			   (mSwimBackwardSpeed, "Swim Backward Speed")
			   (mAirConsumptionRate, "Air Consumption Rate (%/sec)")
			   (mAirReplenishmentRate, "Air Replenishment Rate (%/sec)")
			   (mInAirToSwanDive, "InAir To Swan Dive")
			   (mSwanDive, "Swan Dive")
			   (mSwim, "Swim")
			   (mBackswim, "Backswim")
			   (mSwimToBackswim, "Swim To Backswim")
			   (mBackswimToSwim, "Backswim To Swim")
			   (mTreadWater, "Tread Water")
			   (mIdleUnderwater, "Idle Underwater")
			   (mIdleUWtoTread, "IdleUW to Tread")
			   (mSwimToRun, "Swim To Run")
		   ("Human Sounds")
			   (mSlideSound, "Slide Sound (looped)")
			   (mHardLandingSound, "Hard Landing Sound")
			   (mDragonCallSounds, "Dragon Call Sound")
			   (mHuhSounds, "Huh? Sound")
			   (mCantReachSounds, "Can't Reach Sound")
			   (mCantUseSounds, "Can't Use Sound")
			   (mFallIntoWaterSound, "Fall Into Water Sound")
			   (mInventoryFullSound, "Inventory Full Sound")
			   (mNoDartsSound, "No Darts Sound")
			   (mNoArrowsSound, "No Arrows Sound")
			   (mSheatheSound, "Sheathe Sound")
			   (mPushObjectSounds, "Push Object Sounds")
			   (mAttackGrunts, "Attack Grunts")
		   ("Ambient Comments")
			   (mGetThePointSounds, "Get-the-point Sounds")
			   (mGetThePointSndChance, "Get the point Snd Chance (0-100)");
    }



    HumanControl::HumanControl()
    : mYaw(0)
	, mPrevYaw(0)
	, mPitch(0)
	, mForwardSpeed(0)
	, mRightSpeed(0)
    {

    }

    HumanControl::~HumanControl()
    {
    	if(mUpdateCallback != nullptr && mPlayerObject != nullptr)
		{
			mPlayerObject->removeUpdateCallback(mUpdateCallback);
		}
    }

    void HumanControl::loaded(od::Engine &engine, od::LevelObject *obj)
    {
        if(engine.getPlayer() != nullptr)
        {
            Logger::warn() << "Duplicate HumanControl objects found in level. Ignoring.";
            // FIXME: instead of just ignoring the issue, perhaps request the duplicate objects to be destroyed?
            return;
        }

        mPlayerObject = obj;
        engine.getLevel().setPlayer(this);

        // prefetch animations
        mRunAnim.getOrFetchAsset(mPlayerObject->getClass()->getDatabase());
        mReadyAnim.getOrFetchAsset(mPlayerObject->getClass()->getDatabase());
    }

    void HumanControl::spawned(od::LevelObject &obj)
    {
        if(mPlayerObject == nullptr)
        {
            // this means we ignored the load call because of duplicate HumanControl objects, so we should ignore the spawn as well
            return;
        }

    	Logger::verbose() << "Spawned Human Control at "
    			<< obj.getPosition().x() << "/"
				<< obj.getPosition().y() << "/"
				<< obj.getPosition().z();

    	obj.setEnableRflUpdateHook(true);

    	if(obj.getSkeletonRoot() == nullptr)
    	{
    		Logger::error() << "HumanControl class used on object without skeleton. Prepare for chaos";

    	}else
    	{
    		mCharacterController.reset(new od::CharacterController(obj, 0.05, 0.3));

			mAnimationPlayer = new od::SkeletonAnimationPlayer(obj.getLevel().getEngine(), &obj, obj.getSkeletonRoot(), mCharacterController.get());
    	}
    }

    void HumanControl::update(od::LevelObject &obj, double simTime, double relTime)
    {
        mCharacterController->update(relTime);

        _updateMotion(relTime);
    }

    void HumanControl::moveForward(float speed)
    {
    	mForwardSpeed = speed;
    }

	void HumanControl::moveRight(float speed)
    {
    	mRightSpeed = speed;
    }

	osg::Vec3f HumanControl::getPosition()
    {
    	if(mPlayerObject == nullptr)
    	{
    		return osg::Vec3f(0,0,0);
    	}

    	return mPlayerObject->getPosition();
    }

	od::LevelObject &HumanControl::getLevelObject()
	{
	    return *mPlayerObject;
	}

    void HumanControl::_updateMotion(double relTime)
    {
        osg::Quat rot = osg::Quat(mYaw, osg::Vec3(0, 1, 0));
        mPlayerObject->setRotation(rot);
        mPrevYaw = mYaw;

        if(mCharacterController->getCharacterState() == od::CharacterState::Penetrated_Object)
        {
        	if(mAnimationPlayer->getCurrentAnimation() == mReadyAnim.getAsset())
			{
        		return;
			}

        	mAnimationPlayer->setAnimation(mReadyAnim.getAsset(), 0.15);
			mAnimationPlayer->play(true);

        }else
        {

			/*float yawSpeed = (mYaw - mPrevYaw)/relTime;
			*/

			if(mForwardSpeed > 0 && mAnimationPlayer->getCurrentAnimation() != mRunAnim.getAsset())
			{
				mAnimationPlayer->setAnimation(mRunAnim.getAsset(), 0.01);
				mAnimationPlayer->play(true);

			}else if(mForwardSpeed == 0 && mAnimationPlayer->getCurrentAnimation() != mReadyAnim.getAsset())
			{
				mAnimationPlayer->setAnimation(mReadyAnim.getAsset(), 0.15);
				mAnimationPlayer->play(true);
			}
        }
    }

    OD_REGISTER_RFL_CLASS(0x0009, "Human Control", HumanControl);

}
