/*
 * RflPlayerControl.h
 *
 *  Created on: 16 Feb 2018
 *      Author: zal
 */

#ifndef RFLPLAYERCONTROL_H_
#define RFLPLAYERCONTROL_H_

#include "rfl/RflClass.h"

namespace od
{

    class RflPlayerCommon : public RflClass
    {
    public:

        RflPlayerCommon();

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
        RflClassRefArray    mGibGenLarge;
        RflClassRefArray    mGibGenSmall;
        RflCharChannelArray mLargeGibChans;
        RflFloat            mBodyPartVelocity;
    };

    class RflPlayerControl : public RflPlayerCommon
    {
    public:

        RflPlayerControl();


    protected:



    };

}

#endif /* RFLPLAYERCONTROL_H_ */
