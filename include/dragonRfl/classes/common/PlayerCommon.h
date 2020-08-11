/*
 * PlayerCommon.h
 *
 *  Created on: 16 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_COMMON_PLAYERCOMMON_H_
#define INCLUDE_RFL_COMMON_PLAYERCOMMON_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace dragonRfl
{

    class PlayerCommonFields : public odRfl::FieldBundle
    {
    public:

        PlayerCommonFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::SoundRefArray    separatedSounds;
        odRfl::Integer          separatedSoundChance;
        odRfl::Float            cameraHorzDistance;
        odRfl::Float            cameraVertDistance;
        odRfl::Float            cameraTiltFactor;
        odRfl::Float            cameraCollisionTiltFactor;
        odRfl::Float            maxProximity;
        odRfl::AnimRef          readyAnim;
        odRfl::AnimRef          walkAnim;
        odRfl::AnimRef          runAnim;
        odRfl::AnimRef          groundStrafeLAnim;
        odRfl::AnimRef          groundStrafeRAnim;
        odRfl::AnimRefArray     idleAnims;
        odRfl::Float            runSpeed;
        odRfl::Float            walkSpeed;
        odRfl::Float            walkBackSpeed;
        odRfl::Float            throttleAccel;
        odRfl::Float            throttleDecel;
        odRfl::Float            sideStrafeSpeed;
        odRfl::Float            rollSpeed;
        odRfl::Float            pitchSpeed;
        odRfl::Float            sideAccel;
        odRfl::Float            rollDamping;
        odRfl::Float            pitchDamping;
        odRfl::CharChannel      neckChannel;
        odRfl::CharChannel      headChannel;
        odRfl::Integer          footSphere;
        odRfl::Integer          beginHealth;
        odRfl::ClassRef         gotHitBloodGen;
        odRfl::AnimRefArray     groundDeathAnims;
        odRfl::ClassRef         _3DExplosion;
        odRfl::SoundRef         deathSound;
        odRfl::ClassRef         amputationBloodGen;
        odRfl::ClassRef         bodyPart;
        odRfl::ClassRef         bodyPartLarge;
        odRfl::Float            bodyPartPercentage;
        odRfl::CharChannelArray choppableChansAlive;
        odRfl::Integer          gibCutoff;
        odRfl::ClassRefArray    gibGensLarge;
        odRfl::ClassRefArray    gibGensSmall;
        odRfl::CharChannelArray largeGibChans;
        odRfl::Float            bodyPartVelocity;
    };
}

#endif /* INCLUDE_RFL_COMMON_PLAYERCOMMON_H_ */
