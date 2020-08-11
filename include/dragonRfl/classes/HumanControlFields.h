/*
 * HumanControlFields.h
 *
 *  Created on: 21 Jan 2019
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_CLASSES_HUMANCONTROLFIELDS_H_
#define INCLUDE_DRAGONRFL_CLASSES_HUMANCONTROLFIELDS_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

#include <dragonRfl/classes/common/PlayerCommon.h>

namespace dragonRfl
{
    /**
     * This is located in it's own file so the HumanControl source is not cluttered with the 179 members this field bundle defines.
     */
    class HumanControlFields : public PlayerCommonFields
    {
    public:

        HumanControlFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::CharChannel      rightWeaponChannel;
        odRfl::CharChannel      leftWeaponChannel;
        odRfl::CharChannel      torsoChannel;
        odRfl::CharChannel      lowerBodyChan;
        odRfl::CharChannel      rightArmChan;
        odRfl::CharChannelArray solveToChans;
        odRfl::CharChannelArray leftLegChans;
        odRfl::CharChannelArray rightLegChans;
        odRfl::CharChannel      hair1Channel;
        odRfl::Float            multiplayerSpeedScaling;
        odRfl::Float            fallStartDmgSpeed;
        odRfl::Float            fallDeathSpeed;
        odRfl::AnimRef          drown;
        odRfl::SoundRef         drownSound;
        odRfl::AnimRef          combatIdleOneHand;
        odRfl::AnimRef          combatIdleTwoHand;
        odRfl::AnimRef          walkOneHanded;
        odRfl::AnimRef          runOneHanded;
        odRfl::AnimRef          strafeLeftOneHanded;
        odRfl::AnimRef          strafeRightOneHanded;
        odRfl::AnimRef          walkCombat;
        odRfl::AnimRef          runCombat;
        odRfl::AnimRef          rideAir;
        odRfl::AnimRef          rideGround;
        odRfl::AnimRef          mount;
        odRfl::AnimRef          dismount;
        odRfl::AnimRef          jump;
        odRfl::AnimRef          inAir;
        odRfl::AnimRef          landing;
        odRfl::AnimRef          landingSmall;
        odRfl::AnimRef          landingRun;
        odRfl::AnimRef          noncombatToCrouch;
        odRfl::AnimRef          combatToCrouch;
        odRfl::AnimRef          noncombatCrouched;
        odRfl::AnimRef          combatCrouched;
        odRfl::AnimRef          waddleForward;
        odRfl::AnimRef          rollLeft;
        odRfl::AnimRef          rollRight;
        odRfl::AnimRef          rollForward;
        odRfl::AnimRef          rollBack;
        odRfl::AnimRef          cartwheelL;
        odRfl::AnimRef          cartwheelR;
        odRfl::AnimRef          flipForward;
        odRfl::AnimRef          flipBack;
        odRfl::AnimRef          pushRollable;
        odRfl::AnimRef          pushSlidable;
        odRfl::AnimRef          ladderUp1;
        odRfl::AnimRef          ladderUp2;
        odRfl::AnimRef          ladderDown1;
        odRfl::AnimRef          ladderDown2;
        odRfl::AnimRef          ladderUpLIdle;
        odRfl::AnimRef          ladderUpRIdle;
        odRfl::AnimRef          ladderDownRIdle;
        odRfl::AnimRef          getOnLadderB;
        odRfl::AnimRef          getOffLadderB;
        odRfl::AnimRef          getOnLadderT;
        odRfl::AnimRef          getOffLadderT;
        odRfl::AnimRef          stairsUp;
        odRfl::AnimRef          stairsDown;
        odRfl::AnimRef          stairsUpIdleL;
        odRfl::AnimRef          stairsUpIdleR;
        odRfl::AnimRef          stairsDownIdleL;
        odRfl::AnimRef          stairsDownIdleR;
        odRfl::AnimRef          huh;
        odRfl::AnimRef          _180Turn;
        odRfl::AnimRef          useMagicCrystal;
        odRfl::AnimRef          slideForward;
        odRfl::AnimRef          slideBackward;
        odRfl::AnimRef          balancing;
        odRfl::AnimRef          throw;
        odRfl::AnimRef          shootGun;
        odRfl::AnimRef          reloadGun;
        odRfl::AnimRef          archerMedium;
        odRfl::AnimRef          archerLow;
        odRfl::Float            gunCamHorzDistance;
        odRfl::Float            gunCamVertDistance;
        odRfl::AnimRef          oneHandRH;
        odRfl::AnimRef          oneHandRM1;
        odRfl::AnimRef          oneHandRM2;
        odRfl::AnimRef          oneHandRM3;
        odRfl::AnimRef          oneHandRL;
        odRfl::AnimRef          oneHandSH;
        odRfl::AnimRef          oneHandSM;
        odRfl::AnimRef          oneHandSL;
        odRfl::AnimRef          oneHandUB1;
        odRfl::AnimRef          oneHandUB2;
        odRfl::AnimRef          oneHandUB3;
        odRfl::AnimRef          twoHandRH;
        odRfl::AnimRef          twoHandRM1;
        odRfl::AnimRef          twoHandRM2;
        odRfl::AnimRef          twoHandRM3;
        odRfl::AnimRef          twoHandRL;
        odRfl::AnimRef          twoHandSH;
        odRfl::AnimRef          twoHandSM;
        odRfl::AnimRef          twoHandSL;
        odRfl::AnimRef          twoHandUB1;
        odRfl::AnimRef          twoHandUB2;
        odRfl::AnimRef          twoHandUB3;
        odRfl::AnimRef          fireSA;
        odRfl::AnimRef          iceSA;
        odRfl::AnimRef          lightningSA;
        odRfl::AnimRef          deathSA;
        odRfl::AnimRef          strafeAttackLUB;
        odRfl::AnimRef          strafeAttackRUB;
        odRfl::AnimRef          rollLAttack;
        odRfl::AnimRef          rollRAttack;
        odRfl::AnimRef          specialAttackThrust;
        odRfl::AnimRef          specialAttackLeft;
        odRfl::AnimRef          specialAttackRight;
        odRfl::AnimRef          specialAttackSpin;
        odRfl::AnimRef          specialAttackSwing180;
        odRfl::AnimRef          specialAttackJump;
        odRfl::Float            saThrustDamageMultiplier;
        odRfl::Float            saLeftDamageMultiplier;
        odRfl::Float            saRightDamageMultiplier;
        odRfl::Float            saSpinDamageMultiplier;
        odRfl::Float            saSwing180DamageMultiplier;
        odRfl::Float            saJumpDamageMultiplier;
        odRfl::AnimRef          block;
        odRfl::AnimRef          crouchedBlock;
        odRfl::Integer          blockDamageReductionPercent;
        odRfl::AnimRef          sheathe;
        odRfl::AnimRef          unsheathe;
        odRfl::Float            reAttackWindow;
        odRfl::AnimRef          hitRxnHSLAnim;
        odRfl::AnimRef          hitRxnHSHAnim;
        odRfl::AnimRef          hitRxnOSLAnim;
        odRfl::AnimRef          hitRxnOSHAnim;
        odRfl::AnimRef          hitRxnFSLAnim;
        odRfl::AnimRef          hitRxnFSHAnim;
        odRfl::SoundRefArray    hitRxnHSLSound;
        odRfl::SoundRefArray    hitRxnHSHSound;
        odRfl::SoundRefArray    hitRxnOSLSound;
        odRfl::SoundRefArray    hitRxnOSHSound;
        odRfl::SoundRefArray    hitRxnFSLSound;
        odRfl::SoundRefArray    HitRxnFSHSound;
        odRfl::AnimRef          walkStrafeL;
        odRfl::AnimRef          walkStrafeR;
        odRfl::AnimRef          slowWalk;
        odRfl::AnimRef          runBackwards;
        odRfl::AnimRef          runBackwardsOneHanded;
        odRfl::AnimRef          noncombat2Run;
        odRfl::AnimRef          run2Ready;
        odRfl::AnimRef          run2ReadyL;
        odRfl::AnimRef          walk2Ready;
        odRfl::AnimRef          strafeL2Ready;
        odRfl::AnimRef          strafeR2Ready;
        odRfl::AnimRef          turnLeft;
        odRfl::AnimRef          turnRight;
        odRfl::AnimRef          bumpIntoWall;
        odRfl::Float            swimForwardSpeed;
        odRfl::Float            swimBackwardSpeed;
        odRfl::Float            airConsumptionRate;
        odRfl::Float            airReplenishmentRate;
        odRfl::AnimRef          inAirToSwanDive;
        odRfl::AnimRef          swanDive;
        odRfl::AnimRef          swim;
        odRfl::AnimRef          backswim;
        odRfl::AnimRef          swimToBackswim;
        odRfl::AnimRef          backswimToSwim;
        odRfl::AnimRef          treadWater;
        odRfl::AnimRef          idleUnderwater;
        odRfl::AnimRef          idleUWtoTread;
        odRfl::AnimRef          swimToRun;
        odRfl::SoundRef         slideSound;
        odRfl::SoundRef         hardLandingSound;
        odRfl::SoundRefArray    dragonCallSounds;
        odRfl::SoundRefArray    huhSounds;
        odRfl::SoundRefArray    cantReachSounds;
        odRfl::SoundRefArray    cantUseSounds;
        odRfl::SoundRef         fallIntoWaterSound;
        odRfl::SoundRef         inventoryFullSound;
        odRfl::SoundRef         noDartsSound;
        odRfl::SoundRef         noArrowsSound;
        odRfl::SoundRef         sheatheSound;
        odRfl::SoundRefArray    pushObjectSounds;
        odRfl::SoundRefArray    attackGrunts;
        odRfl::SoundRefArray    getThePointSounds;
        odRfl::Integer          getThePointSndChance;
    };

}

#endif /* INCLUDE_DRAGONRFL_CLASSES_HUMANCONTROLFIELDS_H_ */
