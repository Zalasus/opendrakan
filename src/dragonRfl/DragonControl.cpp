/*
 * DragonControl.cpp
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#include <dragonRfl/DragonControl.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>
#include <odCore/Level.h>
#include <odCore/Player.h>

namespace od
{

	DragonControl::DragonControl()
    : mWalkBackwardAnim(od::AssetRef::NULL_REF)
	, mAirCamHorzDistance(1024.0)
	, mAirCamVertDistance(1024.0)
	, mSaddleChannel(0)
	, mJawChannel(0)
	, mNeck2Channel(0)
	, mBiteDamage(20)
	, mGotHitSounds({})
	, mCrashSounds({})
	, mCrashSoundChance(30)
	, mWingFlapSounds({})
	, mBiteSounds({})
	, mDoneMountingSounds({})
	, mPercentChanceOfDoneMountingSound(25)
	, mAirDeathStartAnim(od::AssetRef::NULL_REF)
	, mAirDeathLoopAnim(od::AssetRef::NULL_REF)
	, mAirDeathImpactAnim(od::AssetRef::NULL_REF)
	, mMountAnim(od::AssetRef::NULL_REF)
	, mDismountAnim(od::AssetRef::NULL_REF)
	, mFlyAnim(od::AssetRef::NULL_REF)
	, mFlyToHoverAnim(od::AssetRef::NULL_REF)
	, mHoverAnim(od::AssetRef::NULL_REF)
	, mHoverBackwardsAnim(od::AssetRef::NULL_REF)
	, mSoarAnim(od::AssetRef::NULL_REF)
	, mStrafeLeftAirAnim(od::AssetRef::NULL_REF)
	, mStrafeRightAirAnim(od::AssetRef::NULL_REF)
	, mStrafeUpAnim(od::AssetRef::NULL_REF)
	, mTurnLeftGndAnim(od::AssetRef::NULL_REF)
	, mTurnRightGndAnim(od::AssetRef::NULL_REF)
	, mLandingAnim(od::AssetRef::NULL_REF)
	, mRunIntoWallAnim(od::AssetRef::NULL_REF)
	, mFlyIntoWallAnim(od::AssetRef::NULL_REF)
	, mBiteAnim(od::AssetRef::NULL_REF)
	, mMinFlySpeed(-1.0)
	, mMaxFlySpeed(2.0)
	, mAirStrafeLRSpeed(3.0)
	, mStartHoverAnimSpeed(0.75)
	, mStartFlyAnimSpeed(1.5)
	, mUpStrafeSpeed(3.5)
	, mUpAccel(4.0)
	, mTurnDamping(8.0)
	, mClimbFactor(0.5)
	, mDiveFactor(2.0)
	, mSoarAngle(0.1)
	, mNoFlyHeadroom(2048.0)
	, mInitialMode(1) // Ground Human
	, mDefaultWeapon(od::AssetRef::NULL_REF)
	, mFollowHumanDistance(10.0)
	, mCantLandSounds({})
	, mCallResponseSounds({})
	, mImCloseEnoughSounds({})
	, mCantFlySounds({})
	, mMeetAtCaveEntranceSounds({})
	, mKilledEnemySounds({})
	, mHealthLowSounds({})
	, mGotHitByRynnSounds({})
	, mGetOutOfMyWaySounds({})
	, mRechargeRatePerSecond(10.0)
	, mTakeOffSounds({})
	, mTakeOffSoundChance(20)
	, mDoneDismountingSounds({})
	, mDoneDismountSndChance(10)
    {
    }

    void DragonControl::probeFields(FieldProbe &probe)
	{
    	PlayerCommon::probeFields(probe);

    	probe.beginCategory("Ground Motions Animations");
    	probe.registerField(mWalkBackwardAnim, "Walk Backward");

    	probe.beginCategory("Camera Control");
		probe.registerField(mAirCamHorzDistance, "AirCam Horz Distance");
		probe.registerField(mAirCamVertDistance, "AirCam Vert Distance");

		probe.beginCategory("Character Channels");
		probe.registerField(mSaddleChannel, "Saddle Channel");
		probe.registerField(mJawChannel, "Jaw Channel");
		probe.registerField(mNeck2Channel, "Neck 2 Channel");

		probe.beginCategory("Health and Damage");
		probe.registerField(mBiteDamage, "Bite Damage");

		probe.beginCategory("General Sounds");
		probe.registerField(mGotHitSounds, "Got Hit Sounds");
		probe.registerField(mCrashSounds, "Crash Sounds");
		probe.registerField(mCrashSoundChance, "Crash Sound Chance (0-100)");
		probe.registerField(mWingFlapSounds, "Wing Flap Sounds");
		probe.registerField(mBiteSounds, "Bite Sounds");
		probe.registerField(mDoneMountingSounds, "Done Mounting Sounds");
		probe.registerField(mPercentChanceOfDoneMountingSound, "Percent Chance of Done Mounting Sound (0-100)");

		probe.beginCategory("Death");
		probe.registerField(mAirDeathStartAnim, "Air Death Start");
		probe.registerField(mAirDeathLoopAnim, "Air Death Loop");
		probe.registerField(mAirDeathImpactAnim, "Air Death Impact");

		probe.beginCategory("Dragon Animations");
		probe.registerField(mMountAnim, "Mount");
		probe.registerField(mDismountAnim, "Dismount");
		probe.registerField(mFlyAnim, "Fly");
		probe.registerField(mFlyToHoverAnim, "Fly-->Hover");
		probe.registerField(mHoverAnim, "Hover");
		probe.registerField(mHoverBackwardsAnim, "Hover Backwards");
		probe.registerField(mSoarAnim, "Soar");
		probe.registerField(mStrafeLeftAirAnim, "Strafe Left Air");
		probe.registerField(mStrafeRightAirAnim, "Strafe Right Air");
		probe.registerField(mStrafeUpAnim, "Strafe Up");
		probe.registerField(mTurnLeftGndAnim, "Turn Left (Gnd)");
		probe.registerField(mTurnRightGndAnim, "Turn Right (Gnd)");
		probe.registerField(mLandingAnim, "Landing");
		probe.registerField(mRunIntoWallAnim, "Run Into Wall");
		probe.registerField(mFlyIntoWallAnim, "Fly Into Wall");
		probe.registerField(mBiteAnim, "Bite");

		probe.beginCategory("Flight Speeds and Factors");
		probe.registerField(mMinFlySpeed, "Min Fly Speed (lu/s)");
		probe.registerField(mMaxFlySpeed, "Max Fly Speed (lu/s)");
		probe.registerField(mAirStrafeLRSpeed, "Air Strafe L/R Speed");
		probe.registerField(mStartHoverAnimSpeed, "Start HoverAnim Speed (lu/s)");
		probe.registerField(mStartFlyAnimSpeed, "Start FlyAnim Speed (lu/s)");
		probe.registerField(mUpStrafeSpeed, "Up Strafe Speed (lu/s)");
		probe.registerField(mUpAccel, "Up Accel (lu/s^2)");
		probe.registerField(mTurnDamping, "Turn Damping");
		probe.registerField(mClimbFactor, "Climb Factor");
		probe.registerField(mDiveFactor, "Dive Factor");
		probe.registerField(mSoarAngle, "Soar Angle (0-1)");
		probe.registerField(mNoFlyHeadroom, "No-Fly Headroom");

		probe.beginCategory("Autonomous AI");
		probe.registerField(mInitialMode, "Initial Mode");
		probe.registerField(mDefaultWeapon, "Default Weapon");
		probe.registerField(mFollowHumanDistance, "Follow Human Distance (lu)");
		probe.registerField(mCantLandSounds, "Can't Land Sounds");
		probe.registerField(mCallResponseSounds, "Call Response Sounds");
		probe.registerField(mImCloseEnoughSounds, "I'm Close Enough Sounds");
		probe.registerField(mCantFlySounds, "Can't Fly Sounds");
		probe.registerField(mMeetAtCaveEntranceSounds, "Meet At Cave Entrance Sounds");
		probe.registerField(mKilledEnemySounds, "Killed Enemy Sounds");
		probe.registerField(mHealthLowSounds, "Health Low Sounds");
		probe.registerField(mGotHitByRynnSounds, "Got Hit By Rynn Sounds");
		probe.registerField(mGetOutOfMyWaySounds, "Get Out of My way! Sounds");

		probe.beginCategory("Energy");
		probe.registerField(mRechargeRatePerSecond, "Recharge Rate per s");

		probe.beginCategory("Ambient Comments");
		probe.registerField(mTakeOffSounds, "Take Off Sounds");
		probe.registerField(mTakeOffSoundChance, "Take Off Sound Chance (0-100)");
		probe.registerField(mDoneDismountingSounds, "Done Dismounting Sounds");
		probe.registerField(mDoneDismountSndChance, "Done Dismount Snd Chance (1-100)");
	}

    OD_REGISTER_RFL_CLASS(DragonRfl, 0x001c, "Player", "Dragon Control", DragonControl);

}
