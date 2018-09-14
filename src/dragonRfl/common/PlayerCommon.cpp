/*
 * PlayerCommon.cpp
 *
 *  Created on: 24 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/common/PlayerCommon.h>

#include <odCore/rfl/Rfl.h>

namespace odRfl
{

	PlayerCommon::PlayerCommon()
	: mSeparatedSounds({})
    , mSeparatedSoundChance(30)
    , mCameraHorzDistance(1024.0)
    , mCameraVertDistance(1024.0)
    , mCameraTiltFactor(0)
    , mCameraCollisionTiltFactor(0.8)
    , mMaxProximity(0.25)
    , mReadyAnim(od::AssetRef::NULL_REF)
    , mWalkAnim(od::AssetRef::NULL_REF)
    , mRunAnim(od::AssetRef::NULL_REF)
    , mGroundStrafeLAnim(od::AssetRef::NULL_REF)
    , mGroundStrafeRAnim(od::AssetRef::NULL_REF)
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
    , mGotHitBloodGen(od::AssetRef::NULL_REF)
    , mGroundDeathAnims({})
    , m3DExplosion(od::AssetRef::NULL_REF)
    , mDeathSound(od::AssetRef::NULL_REF)
    , mAmputationBloodGen(od::AssetRef::NULL_REF)
    , mBodyPart(od::AssetRef::NULL_REF)
    , mBodyPartLarge(od::AssetRef::NULL_REF)
    , mBodyPartPercentage(0.25)
    , mChoppableChansAlive({})
    , mGibCutoff(-5)
    , mGibGensLarge({})
    , mGibGensSmall({})
    , mLargeGibChans({})
    , mBodyPartVelocity(0.43)
	{
	}

    void PlayerCommon::probeFields(RflFieldProbe &probe)
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

}

