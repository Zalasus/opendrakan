/*
 * HumanControl.cpp
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#include <dragonRfl/classes/HumanControl.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/PrefetchProbe.h>
#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/Player.h>
#include <odCore/Engine.h>
#include <odCore/Camera.h>
#include <odCore/anim/SkeletonAnimationPlayer.h>
#include <odCore/audio/SoundManager.h>

namespace dragonRfl
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
	, mDrown(odDb::AssetRef::NULL_REF)
	, mDrownSound(odDb::AssetRef::NULL_REF)
	, mCombatIdleOneHand(odDb::AssetRef::NULL_REF)
	, mCombatIdleTwoHand(odDb::AssetRef::NULL_REF)
	, mWalkOneHanded(odDb::AssetRef::NULL_REF)
	, mRunOneHanded(odDb::AssetRef::NULL_REF)
	, mStrafeLeftOneHanded(odDb::AssetRef::NULL_REF)
	, mStrafeRightOneHanded(odDb::AssetRef::NULL_REF)
	, mWalkCombat(odDb::AssetRef::NULL_REF)
	, mRunCombat(odDb::AssetRef::NULL_REF)
	, mRideAir(odDb::AssetRef::NULL_REF)
	, mRideGround(odDb::AssetRef::NULL_REF)
	, mMount(odDb::AssetRef::NULL_REF)
	, mDismount(odDb::AssetRef::NULL_REF)
	, mJump(odDb::AssetRef::NULL_REF)
	, mInAir(odDb::AssetRef::NULL_REF)
	, mLanding(odDb::AssetRef::NULL_REF)
	, mLandingSmall(odDb::AssetRef::NULL_REF)
	, mLandingRun(odDb::AssetRef::NULL_REF)
	, mNoncombatToCrouch(odDb::AssetRef::NULL_REF)
	, mCombatToCrouch(odDb::AssetRef::NULL_REF)
	, mNoncombatCrouched(odDb::AssetRef::NULL_REF)
	, mCombatCrouched(odDb::AssetRef::NULL_REF)
	, mWaddleForward(odDb::AssetRef::NULL_REF)
	, mRollLeft(odDb::AssetRef::NULL_REF)
	, mRollRight(odDb::AssetRef::NULL_REF)
	, mRollForward(odDb::AssetRef::NULL_REF)
	, mRollBack(odDb::AssetRef::NULL_REF)
	, mCartwheelL(odDb::AssetRef::NULL_REF)
	, mCartwheelR(odDb::AssetRef::NULL_REF)
	, mFlipForward(odDb::AssetRef::NULL_REF)
	, mFlipBack(odDb::AssetRef::NULL_REF)
	, mPushRollable(odDb::AssetRef::NULL_REF)
	, mPushSlidable(odDb::AssetRef::NULL_REF)
	, mLadderUp1(odDb::AssetRef::NULL_REF)
	, mLadderUp2(odDb::AssetRef::NULL_REF)
	, mLadderDown1(odDb::AssetRef::NULL_REF)
	, mLadderDown2(odDb::AssetRef::NULL_REF)
	, mLadderUpLIdle(odDb::AssetRef::NULL_REF)
	, mLadderUpRIdle(odDb::AssetRef::NULL_REF)
	, mLadderDownRIdle(odDb::AssetRef::NULL_REF)
	, mGetOnLadderB(odDb::AssetRef::NULL_REF)
	, mGetOffLadderB(odDb::AssetRef::NULL_REF)
	, mGetOnLadderT(odDb::AssetRef::NULL_REF)
	, mGetOffLadderT(odDb::AssetRef::NULL_REF)
	, mStairsUp(odDb::AssetRef::NULL_REF)
	, mStairsDown(odDb::AssetRef::NULL_REF)
	, mStairsUpIdleL(odDb::AssetRef::NULL_REF)
	, mStairsUpIdleR(odDb::AssetRef::NULL_REF)
	, mStairsDownIdleL(odDb::AssetRef::NULL_REF)
	, mStairsDownIdleR(odDb::AssetRef::NULL_REF)
	, mHuh(odDb::AssetRef::NULL_REF)
	, m180Turn(odDb::AssetRef::NULL_REF)
	, mUseMagicCrystal(odDb::AssetRef::NULL_REF)
	, mSlideForward(odDb::AssetRef::NULL_REF)
	, mSlideBackward(odDb::AssetRef::NULL_REF)
	, mBalancing(odDb::AssetRef::NULL_REF)
	, mThrow(odDb::AssetRef::NULL_REF)
	, mShootGun(odDb::AssetRef::NULL_REF)
	, mReloadGun(odDb::AssetRef::NULL_REF)
	, mArcherMedium(odDb::AssetRef::NULL_REF)
	, mArcherLow(odDb::AssetRef::NULL_REF)
	, mGunCamHorzDistance(1024.0)
	, mGunCamVertDistance(1024.0)
	, mOneHandRH(odDb::AssetRef::NULL_REF)
	, mOneHandRM1(odDb::AssetRef::NULL_REF)
	, mOneHandRM2(odDb::AssetRef::NULL_REF)
	, mOneHandRM3(odDb::AssetRef::NULL_REF)
	, mOneHandRL(odDb::AssetRef::NULL_REF)
	, mOneHandSH(odDb::AssetRef::NULL_REF)
	, mOneHandSM(odDb::AssetRef::NULL_REF)
	, mOneHandSL(odDb::AssetRef::NULL_REF)
	, mOneHandUB1(odDb::AssetRef::NULL_REF)
	, mOneHandUB2(odDb::AssetRef::NULL_REF)
	, mOneHandUB3(odDb::AssetRef::NULL_REF)
	, mTwoHandRH(odDb::AssetRef::NULL_REF)
	, mTwoHandRM1(odDb::AssetRef::NULL_REF)
	, mTwoHandRM2(odDb::AssetRef::NULL_REF)
	, mTwoHandRM3(odDb::AssetRef::NULL_REF)
	, mTwoHandRL(odDb::AssetRef::NULL_REF)
	, mTwoHandSH(odDb::AssetRef::NULL_REF)
	, mTwoHandSM(odDb::AssetRef::NULL_REF)
	, mTwoHandSL(odDb::AssetRef::NULL_REF)
	, mTwoHandUB1(odDb::AssetRef::NULL_REF)
	, mTwoHandUB2(odDb::AssetRef::NULL_REF)
	, mTwoHandUB3(odDb::AssetRef::NULL_REF)
	, mFireSA(odDb::AssetRef::NULL_REF)
	, mIceSA(odDb::AssetRef::NULL_REF)
	, mLightningSA(odDb::AssetRef::NULL_REF)
	, mDeathSA(odDb::AssetRef::NULL_REF)
	, mStrafeAttackLUB(odDb::AssetRef::NULL_REF)
	, mStrafeAttackRUB(odDb::AssetRef::NULL_REF)
	, mRollLAttack(odDb::AssetRef::NULL_REF)
	, mRollRAttack(odDb::AssetRef::NULL_REF)
	, mSpecialAttackThrust(odDb::AssetRef::NULL_REF)
	, mSpecialAttackLeft(odDb::AssetRef::NULL_REF)
	, mSpecialAttackRight(odDb::AssetRef::NULL_REF)
	, mSpecialAttackSpin(odDb::AssetRef::NULL_REF)
	, mSpecialAttackSwing180(odDb::AssetRef::NULL_REF)
	, mSpecialAttackJump(odDb::AssetRef::NULL_REF)
	, mSAThrustDamageMultiplier(1.25)
	, mSALeftDamageMultiplier(1.25)
	, mSARightDamageMultiplier(1.25)
	, mSASpinDamageMultiplier(1.25)
	, mSASwing180DamageMultiplier(1.25)
	, mSAJumpDamageMultiplier(1.25)
	, mBlock(odDb::AssetRef::NULL_REF)
	, mCrouchedBlock(odDb::AssetRef::NULL_REF)
	, mBlockDamageReductionPercent(25)
	, mSheathe(odDb::AssetRef::NULL_REF)
	, mUnsheathe(odDb::AssetRef::NULL_REF)
	, mReAttackWindow(0.2)
	, mHitRxnHSLAnim(odDb::AssetRef::NULL_REF)
	, mHitRxnHSHAnim(odDb::AssetRef::NULL_REF)
	, mHitRxnOSLAnim(odDb::AssetRef::NULL_REF)
	, mHitRxnOSHAnim(odDb::AssetRef::NULL_REF)
	, mHitRxnFSLAnim(odDb::AssetRef::NULL_REF)
	, mHitRxnFSHAnim(odDb::AssetRef::NULL_REF)
	, mHitRxnHSLSound({})
	, mHitRxnHSHSound({})
	, mHitRxnOSLSound({})
	, mHitRxnOSHSound({})
	, mHitRxnFSLSound({})
	, mHitRxnFSHSound({})
	, mWalkStrafeL(odDb::AssetRef::NULL_REF)
	, mWalkStrafeR(odDb::AssetRef::NULL_REF)
	, mSlowWalk(odDb::AssetRef::NULL_REF)
	, mRunBackwards(odDb::AssetRef::NULL_REF)
	, mRunBackwardsOneHanded(odDb::AssetRef::NULL_REF)
	, mNoncombat2Run(odDb::AssetRef::NULL_REF)
	, mRun2Ready(odDb::AssetRef::NULL_REF)
	, mRun2ReadyL(odDb::AssetRef::NULL_REF)
	, mWalk2Ready(odDb::AssetRef::NULL_REF)
	, mStrafeL2Ready(odDb::AssetRef::NULL_REF)
	, mStrafeR2Ready(odDb::AssetRef::NULL_REF)
	, mTurnLeft(odDb::AssetRef::NULL_REF)
	, mTurnRight(odDb::AssetRef::NULL_REF)
	, mBumpIntoWall(odDb::AssetRef::NULL_REF)
	, mSwimForwardSpeed(1.2)
	, mSwimBackwardSpeed(-0.7)
	, mAirConsumptionRate(2.0)
	, mAirReplenishmentRate(2.0)
	, mInAirToSwanDive(odDb::AssetRef::NULL_REF)
	, mSwanDive(odDb::AssetRef::NULL_REF)
	, mSwim(odDb::AssetRef::NULL_REF)
	, mBackswim(odDb::AssetRef::NULL_REF)
	, mSwimToBackswim(odDb::AssetRef::NULL_REF)
	, mBackswimToSwim(odDb::AssetRef::NULL_REF)
	, mTreadWater(odDb::AssetRef::NULL_REF)
	, mIdleUnderwater(odDb::AssetRef::NULL_REF)
	, mIdleUWtoTread(odDb::AssetRef::NULL_REF)
	, mSwimToRun(odDb::AssetRef::NULL_REF)
	, mSlideSound(odDb::AssetRef::NULL_REF)
	, mHardLandingSound(odDb::AssetRef::NULL_REF)
	, mDragonCallSounds({})
	, mHuhSounds({})
	, mCantReachSounds({})
	, mCantUseSounds({})
	, mFallIntoWaterSound(odDb::AssetRef::NULL_REF)
	, mInventoryFullSound(odDb::AssetRef::NULL_REF)
	, mNoDartsSound(odDb::AssetRef::NULL_REF)
	, mNoArrowsSound(odDb::AssetRef::NULL_REF)
	, mSheatheSound(odDb::AssetRef::NULL_REF)
	, mPushObjectSounds({})
	, mAttackGrunts({})
	, mGetThePointSounds({})
	, mGetThePointSndChance(50)
    {
    }

	HumanControlFields::~HumanControlFields()
	{
	}

    void HumanControlFields::probeFields(odRfl::FieldProbe &probe)
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



    HumanControl::HumanControl(DragonRfl &rfl)
    : mYaw(0)
	, mPitch(0)
    , mPrevYaw(0)
	, mForwardSpeed(0)
	, mRightSpeed(0)
    , mPlayerObject(nullptr)
    {

    }

    HumanControl::~HumanControl()
    {
    }

    void HumanControl::onLoaded(od::LevelObject &obj)
    {
        od::Engine &engine = obj.getLevel().getEngine();

        if(engine.getPlayer() != nullptr)
        {
            Logger::warn() << "Duplicate HumanControl objects found in level. Destroying duplicate";
            obj.requestDestruction();
            return;
        }

        obj.setSpawnStrategy(od::SpawnStrategy::Always);

        mPlayerObject = &obj;
        engine.setPlayer(this);

        // prefetch referenced assets
        odRfl::PrefetchProbe probe(mPlayerObject->getClass()->getAssetProvider());
        this->probeFields(probe);
    }

    void HumanControl::onSpawned(od::LevelObject &obj)
    {
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
    		mCharacterController.reset(new odPhysics::CharacterController(obj, 0.05, 0.3));

			mAnimationPlayer = new odAnim::SkeletonAnimationPlayer(obj.getCachedNode(), obj.getSkeletonRoot(), mCharacterController.get());
    	}
    }

    void HumanControl::onUpdate(od::LevelObject &obj, double simTime, double relTime)
    {
        mCharacterController->update(relTime);

        _updateMotion(relTime);
    }

    void HumanControl::onMoved(od::LevelObject &obj)
    {
        osg::Vec3 pos = obj.getPosition();

        osg::Quat lookDirection = obj.getRotation();
        osg::Vec3 at = lookDirection * osg::Vec3f(0, 0, -1);
        osg::Vec3 up = osg::Vec3f(0, 1, 0);

        odAudio::SoundManager &sm = obj.getLevel().getEngine().getSoundManager();
        sm.setListenerPosition(pos.x(), pos.y(), pos.z());
        sm.setListenerOrientation(at, up);
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

        if(mCharacterController->getCharacterState() == odPhysics::CharacterState::Penetrated_Object)
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


    OD_REGISTER_RFLCLASS(DragonRfl, HumanControl);

}
