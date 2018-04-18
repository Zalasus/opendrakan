/*
 * PushableObject.h
 *
 *  Created on: 18 Apr 2018
 *      Author: zal
 */

#ifndef PUSHABLEOBJECT_H_
#define PUSHABLEOBJECT_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"

namespace odRfl
{

    class PushableObject : public RflClass
    {
    public:

        PushableObject();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawn(od::LevelObject &obj) override;
        virtual ObjectPhysicsType getPhysicsType() override;


    protected:

        RflEnumYesNo       mWaitForTrigger;
        RflEnumYesNo       mBurnable;
        RflSoundRefArray   mBounceSounds;
        RflSoundRef        mLoopedRollSound;
        RflEnumYesNo       mDetectOtherPushables;
        RflClassRef        mSpecialEffect;
        RflEnum            mWaterEffects;
        RflEnumYesNo       mGory;
        RflEnum            mPushMode;
        RflEnum            mPivotPoint;
        RflFloat           mFriction;
        RflFloat           mElasticity;
        RflEnumYesNo       mRigidBody;
        RflFloat           mLifeTime;
        RflInteger         mHealth;
        RflClassRef        mExplosionGenerator;
        RflInteger         mDamage;
        RflEnum            mDamagePlayer;

    };

}


#endif /* PUSHABLEOBJECT_H_ */
