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
        anim[]      Idle
        float       Run Speed (lu/s)
        float       Walk Speed (lu/s)
        float       Walk Back Speed (lu/s)
        float       Throttle Accel (lu/s^2)
        float       Throttle Decel (lu/s^2)
        float       Side Strafe Speed (lu/s)
        float       Roll Speed (rot/s)
        float       Pitch Speed (rot/s)
        float       Side Accel (lu/s^2)
        float       Roll Damping
        float       Pitch Damping
        channel     Neck Channel
        channel     Head Channel
        int32       Foot Sphere
        int32       Begin Health
        class       Got Hit Blood Gen.
        anim[]      Ground Death
        class       3D Explosion
        sound       Death Sound
        class       Amputation Blood Gen.
        class       Body Part
        class       Body Part (large)
        float       Body Part Percentage
        channel[]       Choppable Chan's (alive)
        int32       Gib Cutoff
        class[]     Gib Gen. (large)
        class[]     Gib Gen. (small)
        channel[]       Lrg. Gib Chan's
        float       Body Part Velocity (0-1)
    };

    class RflPlayerControl : public RflPlayerCommon
    {
    public:

        RflPlayerControl(RflFieldProbe &probe) : RflClass(probe) {}


    protected:



    };

}

#endif /* RFLPLAYERCONTROL_H_ */
