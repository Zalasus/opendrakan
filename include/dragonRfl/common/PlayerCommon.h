/*
 * PlayerCommon.h
 *
 *  Created on: 16 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_COMMON_PLAYERCOMMON_H_
#define INCLUDE_RFL_COMMON_PLAYERCOMMON_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace dragonRfl
{

    class PlayerCommon : public odRfl::RflClass
    {
    public:

        PlayerCommon();

        virtual void probeFields(odRfl::FieldProbe &probe) override;


    protected:

        odRfl::RflSoundRefArray    mSeparatedSounds;
        odRfl::RflInteger          mSeparatedSoundChance;
        odRfl::RflFloat            mCameraHorzDistance;
        odRfl::RflFloat            mCameraVertDistance;
        odRfl::RflFloat            mCameraTiltFactor;
        odRfl::RflFloat            mCameraCollisionTiltFactor;
        odRfl::RflFloat            mMaxProximity;
        odRfl::RflAnimRef          mReadyAnim;
        odRfl::RflAnimRef          mWalkAnim;
        odRfl::RflAnimRef          mRunAnim;
        odRfl::RflAnimRef          mGroundStrafeLAnim;
        odRfl::RflAnimRef          mGroundStrafeRAnim;
        odRfl::RflAnimRefArray     mIdleAnims;
        odRfl::RflFloat            mRunSpeed;
        odRfl::RflFloat            mWalkSpeed;
        odRfl::RflFloat            mWalkBackSpeed;
        odRfl::RflFloat            mThrottleAccel;
        odRfl::RflFloat            mThrottleDecel;
        odRfl::RflFloat            mSideStrafeSpeed;
        odRfl::RflFloat            mRollSpeed;
        odRfl::RflFloat            mPitchSpeed;
        odRfl::RflFloat            mSideAccel;
        odRfl::RflFloat            mRollDamping;
        odRfl::RflFloat            mPitchDamping;
        odRfl::RflCharChannel      mNeckChannel;
        odRfl::RflCharChannel      mHeadChannel;
        odRfl::RflInteger          mFootSphere;
        odRfl::RflInteger          mBeginHealth;
        odRfl::RflClassRef         mGotHitBloodGen;
        odRfl::RflAnimRefArray     mGroundDeathAnims;
        odRfl::RflClassRef         m3DExplosion;
        odRfl::RflSoundRef         mDeathSound;
        odRfl::RflClassRef         mAmputationBloodGen;
        odRfl::RflClassRef         mBodyPart;
        odRfl::RflClassRef         mBodyPartLarge;
        odRfl::RflFloat            mBodyPartPercentage;
        odRfl::RflCharChannelArray mChoppableChansAlive;
        odRfl::RflInteger          mGibCutoff;
        odRfl::RflClassRefArray    mGibGensLarge;
        odRfl::RflClassRefArray    mGibGensSmall;
        odRfl::RflCharChannelArray mLargeGibChans;
        odRfl::RflFloat            mBodyPartVelocity;
    };
}

#endif /* INCLUDE_RFL_COMMON_PLAYERCOMMON_H_ */
