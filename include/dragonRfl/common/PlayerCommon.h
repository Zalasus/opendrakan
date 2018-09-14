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

namespace odRfl
{

    class PlayerCommon : public RflClass
    {
    public:

        PlayerCommon();

        virtual void probeFields(RflFieldProbe &probe) override;


    protected:

        RflSoundRefArray    mSeparatedSounds;
        RflInteger          mSeparatedSoundChance;
        RflFloat            mCameraHorzDistance;
        RflFloat            mCameraVertDistance;
        RflFloat            mCameraTiltFactor;
        RflFloat            mCameraCollisionTiltFactor;
        RflFloat            mMaxProximity;
        RflAnimRef          mReadyAnim;
        RflAnimRef          mWalkAnim;
        RflAnimRef          mRunAnim;
        RflAnimRef          mGroundStrafeLAnim;
        RflAnimRef          mGroundStrafeRAnim;
        RflAnimRefArray     mIdleAnims;
        RflFloat            mRunSpeed;
        RflFloat            mWalkSpeed;
        RflFloat            mWalkBackSpeed;
        RflFloat            mThrottleAccel;
        RflFloat            mThrottleDecel;
        RflFloat            mSideStrafeSpeed;
        RflFloat            mRollSpeed;
        RflFloat            mPitchSpeed;
        RflFloat            mSideAccel;
        RflFloat            mRollDamping;
        RflFloat            mPitchDamping;
        RflCharChannel      mNeckChannel;
        RflCharChannel      mHeadChannel;
        RflInteger          mFootSphere;
        RflInteger          mBeginHealth;
        RflClassRef         mGotHitBloodGen;
        RflAnimRefArray     mGroundDeathAnims;
        RflClassRef         m3DExplosion;
        RflSoundRef         mDeathSound;
        RflClassRef         mAmputationBloodGen;
        RflClassRef         mBodyPart;
        RflClassRef         mBodyPartLarge;
        RflFloat            mBodyPartPercentage;
        RflCharChannelArray mChoppableChansAlive;
        RflInteger          mGibCutoff;
        RflClassRefArray    mGibGensLarge;
        RflClassRefArray    mGibGensSmall;
        RflCharChannelArray mLargeGibChans;
        RflFloat            mBodyPartVelocity;
    };
}

#endif /* INCLUDE_RFL_COMMON_PLAYERCOMMON_H_ */
