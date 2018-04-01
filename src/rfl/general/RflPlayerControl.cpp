/*
 * RflPlayerControl.cpp
 *
 *  Created on: 24 Feb 2018
 *      Author: zal
 */

#include "rfl/general/RflPlayerControl.h"

#include "rfl/Rfl.h"
#include "LevelObject.h"
#include "Level.h"
#include "Player.h"

namespace od
{

	RflPlayerCommon::RflPlayerCommon()
	: mSeparatedSounds({})
    , mSeparatedSoundChance(30)
    , mCameraHorzDistance(1024.0)
    , mCameraVertDistance(1024.0)
    , mCameraTiltFactor(0)
    , mCameraCollisionTiltFactor(0.8)
    , mMaxProximity(0.25)
    , mReadyAnim(AssetRef::NULL_REF)
    , mWalkAnim(AssetRef::NULL_REF)
    , mRunAnim(AssetRef::NULL_REF)
    , mGroundStrafeLAnim(AssetRef::NULL_REF)
    , mGroundStrafeRAnim(AssetRef::NULL_REF)
    , mIdleAnims({})
    , mRunSpeed(0.7)
    , mWalkSpeed(0.4)
    , mWalkBackSpeed(-0.4)
    , mThrottleAccel(5.0)
    , mThrottleDecel(5.0)
    , mSideStrafeSpeed(6.0)
    , mRollSpeed(1.0)
    , mPitchSpeed(0.85)
    , mSideAccel(4.0)
    , mRollDamping(8.0)
    , mPitchDamping(6.0)
    , mNeckChannel(0)
    , mHeadChannel(0)
    , mFootSphere(-1)
    , mBeginHealth(100)
    , mGotHitBloodGen(AssetRef::NULL_REF)
    , mGroundDeathAnims({})
    , m3DExplosion(AssetRef::NULL_REF)
    , mDeathSound(AssetRef::NULL_REF)
    , mAmputationBloodGen(AssetRef::NULL_REF)
    , mBodyPart(AssetRef::NULL_REF)
    , mBodyPartLarge(AssetRef::NULL_REF)
    , mBodyPartPercentage(0.25)
    , mChoppableChansAlive({})
    , mGibCutoff(-5)
    , mGibGensLarge({})
    , mGibGensSmall({})
    , mLargeGibChans({})
    , mBodyPartVelocity(0.43)
	{
	}

    void RflPlayerCommon::probeFields(RflFieldProbe &probe)
    {
    	probe.beginCategory("Ambient Comments");
    	probe.registerField(mSeparatedSounds, "Separated Sounds");
		probe.registerField(mSeparatedSoundChance, "Separated Sound Chance (0-100)");

		probe.beginCategory("Camera Control");
		probe.registerField(mCameraHorzDistance, "Camera Horz Distance");
		probe.registerField(mCameraVertDistance, "Camera Vert Distance");
		probe.registerField(mCameraTiltFactor, "Camera Tilt Factor (0-1)");
		probe.registerField(mCameraCollisionTiltFactor, "Camera Collision Tilt Factor (0-1)");
		probe.registerField(mMaxProximity, "Max Proximity (0-1)");

		probe.beginCategory("Ground Motion Animations");
		probe.registerField(mReadyAnim, "Ready");
		probe.registerField(mWalkAnim, "Walk");
		probe.registerField(mRunAnim, "Run");
		probe.registerField(mGroundStrafeLAnim, "Ground Strafe L");
		probe.registerField(mGroundStrafeRAnim, "Ground Strafe R");
		probe.registerField(mIdleAnims, "Idle");

		probe.beginCategory("General Motions Speeds");
		probe.registerField(mRunSpeed, "Run Speed (lu/s)");
		probe.registerField(mWalkSpeed, "Walk Speed (lu/s)");
		probe.registerField(mWalkBackSpeed, "Walk Back Speed (lu/s)");
		probe.registerField(mThrottleAccel, "Throttle Accel (lu/s^2)");
		probe.registerField(mThrottleDecel, "Throttle Decel (lu/s^2)");
		probe.registerField(mSideStrafeSpeed, "Side Strafe Speed (lu/s)");
		probe.registerField(mRollSpeed, "Roll Speed (rot/s)");
		probe.registerField(mPitchSpeed, "Pitch Speed (rot/s)");
		probe.registerField(mSideAccel, "Side Accel (lu/s^2)");
		probe.registerField(mRollDamping, "Roll Damping");
		probe.registerField(mPitchDamping, "Pitch Damping");

		probe.beginCategory("Character Channels");
		probe.registerField(mNeckChannel, "Neck Channel");
		probe.registerField(mHeadChannel, "Head Channel");

		probe.beginCategory("Bounding Spheres");
		probe.registerField(mFootSphere, "Foot Sphere");

		probe.beginCategory("Health And Damage");
		probe.registerField(mBeginHealth, "Begin Health");
		probe.registerField(mGotHitBloodGen, "Got Hit Blood Gen.");

		probe.beginCategory("Death");
		probe.registerField(mGroundDeathAnims, "Ground Death");
		probe.registerField(m3DExplosion, "3D Explosion");
		probe.registerField(mDeathSound, "Death Sound");

		probe.beginCategory("Blood & Amputation");
		probe.registerField(mAmputationBloodGen, "Amputation Blood Gen.");
		probe.registerField(mBodyPart, "Body Part");
		probe.registerField(mBodyPartLarge, "Body Part (large)");
		probe.registerField(mBodyPartPercentage, "Body Part Percentage");
		probe.registerField(mChoppableChansAlive, "Choppable Chan's (alive)");
		probe.registerField(mGibCutoff, "Gib Cutoff");
		probe.registerField(mGibGensLarge, "Gib Gen. (large)");
		probe.registerField(mGibGensSmall, "Gib Gen. (small)");
		probe.registerField(mLargeGibChans, "Lrg. Gib Chan's");
		probe.registerField(mBodyPartVelocity, "Body Part Velocity (0-1)");
    }





    RflDragonControl::RflDragonControl()
    : mWalkBackwardAnim(AssetRef::NULL_REF)
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
	, mAirDeathStartAnim(AssetRef::NULL_REF)
	, mAirDeathLoopAnim(AssetRef::NULL_REF)
	, mAirDeathImpactAnim(AssetRef::NULL_REF)
	, mMountAnim(AssetRef::NULL_REF)
	, mDismountAnim(AssetRef::NULL_REF)
	, mFlyAnim(AssetRef::NULL_REF)
	, mFlyToHoverAnim(AssetRef::NULL_REF)
	, mHoverAnim(AssetRef::NULL_REF)
	, mHoverBackwardsAnim(AssetRef::NULL_REF)
	, mSoarAnim(AssetRef::NULL_REF)
	, mStrafeLeftAirAnim(AssetRef::NULL_REF)
	, mStrafeRightAirAnim(AssetRef::NULL_REF)
	, mStrafeUpAnim(AssetRef::NULL_REF)
	, mTurnLeftGndAnim(AssetRef::NULL_REF)
	, mTurnRightGndAnim(AssetRef::NULL_REF)
	, mLandingAnim(AssetRef::NULL_REF)
	, mRunIntoWallAnim(AssetRef::NULL_REF)
	, mFlyIntoWallAnim(AssetRef::NULL_REF)
	, mBiteAnim(AssetRef::NULL_REF)
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
	, mDefaultWeapon(AssetRef::NULL_REF)
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

    void RflDragonControl::probeFields(RflFieldProbe &probe)
	{
    	RflPlayerCommon::probeFields(probe);

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

    OD_REGISTER_RFL_CLASS(0x001c, "Dragon Control", RflDragonControl);




    class PlayerUpdateCallback : public osg::NodeCallback
	{
	public:

		PlayerUpdateCallback(Player &player)
		: mPlayer(player)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			traverse(node, nv);

			if(nv->getFrameStamp() != nullptr)
			{
				mPlayer.update(nv->getFrameStamp()->getSimulationTime());
			}
		}


	private:

		Player &mPlayer;

	};



    RflHumanControl::RflHumanControl()
    {

    }

    void RflHumanControl::probeFields(RflFieldProbe &probe)
    {
    	RflPlayerCommon::probeFields(probe);
    }

    void RflHumanControl::spawn(LevelObject &obj)
    {
    	// all aboard the indirection train!
    	obj.getLevel().getPlayer().setLevelObject(LevelObjectPtr(&obj));

    	Logger::verbose() << "Spawned Human Control at "
    			<< obj.getPosition().x() << "/"
				<< obj.getPosition().y() << "/"
				<< obj.getPosition().z();

    	obj.addUpdateCallback(new PlayerUpdateCallback(obj.getLevel().getPlayer()));
    }

    OD_REGISTER_RFL_CLASS(0x0009, "Human Control", RflHumanControl);
}

