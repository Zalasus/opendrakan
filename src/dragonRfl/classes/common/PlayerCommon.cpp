/*
 * PlayerCommon.cpp
 *
 *  Created on: 24 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/classes/common/PlayerCommon.h>

#include <odCore/rfl/Rfl.h>

namespace dragonRfl
{

	PlayerCommonFields::PlayerCommonFields()
	: separatedSounds({})
    , separatedSoundChance(30)
    , cameraHorzDistance(1024.0)
    , cameraVertDistance(1024.0)
    , cameraTiltFactor(0)
    , cameraCollisionTiltFactor(0.8)
    , maxProximity(0.25)
    , readyAnim(odDb::AssetRef::NULL_REF)
    , walkAnim(odDb::AssetRef::NULL_REF)
    , runAnim(odDb::AssetRef::NULL_REF)
    , groundStrafeLAnim(odDb::AssetRef::NULL_REF)
    , groundStrafeRAnim(odDb::AssetRef::NULL_REF)
    , idleAnims({})
    , runSpeed(0.7)
    , walkSpeed(0.4)
    , walkBackSpeed(-0.4)
    , throttleAccel(5.0)
    , throttleDecel(5.0)
    , sideStrafeSpeed(6.0)
    , rollSpeed(1.0)
    , pitchSpeed(0.85)
    , sideAccel(4.0)
    , rollDamping(8.0)
    , pitchDamping(6.0)
    , neckChannel(0)
    , headChannel(0)
    , footSphere(-1)
    , beginHealth(100)
    , gotHitBloodGen(odDb::AssetRef::NULL_REF)
    , groundDeathAnims({})
    , _3DExplosion(odDb::AssetRef::NULL_REF)
    , deathSound(odDb::AssetRef::NULL_REF)
    , amputationBloodGen(odDb::AssetRef::NULL_REF)
    , bodyPart(odDb::AssetRef::NULL_REF)
    , bodyPartLarge(odDb::AssetRef::NULL_REF)
    , bodyPartPercentage(0.25)
    , choppableChansAlive({})
    , gibCutoff(-5)
    , gibGensLarge({})
    , gibGensSmall({})
    , largeGibChans({})
    , bodyPartVelocity(0.43)
	{
	}

    void PlayerCommonFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe
        	("Ambient Comments")
            	(separatedSounds, "Separated Sounds")
        		(separatedSoundChance, "Separated Sound Chance (0-100)")
    		("Camera Control")
        		(cameraHorzDistance, "Camera Horz Distance")
        		(cameraVertDistance, "Camera Vert Distance")
        		(cameraTiltFactor, "Camera Tilt Factor (0-1)")
        		(cameraCollisionTiltFactor, "Camera Collision Tilt Factor (0-1)")
        		(maxProximity, "Max Proximity (0-1)")
    		("Ground Motion Animations")
        		(readyAnim, "Ready")
        		(walkAnim, "Walk")
        		(runAnim, "Run")
        		(groundStrafeLAnim, "Ground Strafe L")
        		(groundStrafeRAnim, "Ground Strafe R")
        		(idleAnims, "Idle")
    		("General Motions Speeds")
        		(runSpeed, "Run Speed (lu/s)")
        		(walkSpeed, "Walk Speed (lu/s)")
        		(walkBackSpeed, "Walk Back Speed (lu/s)")
        		(throttleAccel, "Throttle Accel (lu/s^2)")
        		(throttleDecel, "Throttle Decel (lu/s^2)")
        		(sideStrafeSpeed, "Side Strafe Speed (lu/s)")
        		(rollSpeed, "Roll Speed (rot/s)")
        		(pitchSpeed, "Pitch Speed (rot/s)")
        		(sideAccel, "Side Accel (lu/s^2)")
        		(rollDamping, "Roll Damping")
        		(pitchDamping, "Pitch Damping")
    		("Character Channels")
        		(neckChannel, "Neck Channel")
        		(headChannel, "Head Channel")
    		("Bounding Spheres")
    		    (footSphere, "Foot Sphere")
    		("Health And Damage")
        		(beginHealth, "Begin Health")
        		(gotHitBloodGen, "Got Hit Blood Gen.")
    		("Death")
        		(groundDeathAnims, "Ground Death")
        		(_3DExplosion, "3D Explosion")
        		(deathSound, "Death Sound")
    		("Blood & Amputation")
        		(amputationBloodGen, "Amputation Blood Gen.")
        		(bodyPart, "Body Part")
        		(bodyPartLarge, "Body Part (large)")
        		(bodyPartPercentage, "Body Part Percentage")
        		(choppableChansAlive, "Choppable Chan's (alive)")
        		(gibCutoff, "Gib Cutoff")
        		(gibGensLarge, "Gib Gen. (large)")
        		(gibGensSmall, "Gib Gen. (small)")
        		(largeGibChans, "Lrg. Gib Chan's")
        		(bodyPartVelocity, "Body Part Velocity (0-1)");
    }

}
