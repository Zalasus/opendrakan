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

    class PlayerCommon : public odRfl::LevelObjectClassBase
    {
    public:

        PlayerCommon();

        virtual void probeFields(odRfl::FieldProbe &probe) override;


    protected:

        odRfl::SoundRefArray    mSeparatedSounds;
        odRfl::Integer          mSeparatedSoundChance;
        odRfl::Float            mCameraHorzDistance;
        odRfl::Float            mCameraVertDistance;
        odRfl::Float            mCameraTiltFactor;
        odRfl::Float            mCameraCollisionTiltFactor;
        odRfl::Float            mMaxProximity;
        odRfl::AnimRef          mReadyAnim;
        odRfl::AnimRef          mWalkAnim;
        odRfl::AnimRef          mRunAnim;
        odRfl::AnimRef          mGroundStrafeLAnim;
        odRfl::AnimRef          mGroundStrafeRAnim;
        odRfl::AnimRefArray     mIdleAnims;
        odRfl::Float            mRunSpeed;
        odRfl::Float            mWalkSpeed;
        odRfl::Float            mWalkBackSpeed;
        odRfl::Float            mThrottleAccel;
        odRfl::Float            mThrottleDecel;
        odRfl::Float            mSideStrafeSpeed;
        odRfl::Float            mRollSpeed;
        odRfl::Float            mPitchSpeed;
        odRfl::Float            mSideAccel;
        odRfl::Float            mRollDamping;
        odRfl::Float            mPitchDamping;
        odRfl::CharChannel      mNeckChannel;
        odRfl::CharChannel      mHeadChannel;
        odRfl::Integer          mFootSphere;
        odRfl::Integer          mBeginHealth;
        odRfl::ClassRef         mGotHitBloodGen;
        odRfl::AnimRefArray     mGroundDeathAnims;
        odRfl::ClassRef         m3DExplosion;
        odRfl::SoundRef         mDeathSound;
        odRfl::ClassRef         mAmputationBloodGen;
        odRfl::ClassRef         mBodyPart;
        odRfl::ClassRef         mBodyPartLarge;
        odRfl::Float            mBodyPartPercentage;
        odRfl::CharChannelArray mChoppableChansAlive;
        odRfl::Integer          mGibCutoff;
        odRfl::ClassRefArray    mGibGensLarge;
        odRfl::ClassRefArray    mGibGensSmall;
        odRfl::CharChannelArray mLargeGibChans;
        odRfl::Float            mBodyPartVelocity;
    };
}

#endif /* INCLUDE_RFL_COMMON_PLAYERCOMMON_H_ */
