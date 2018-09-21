/*
 * PushableObject.h
 *
 *  Created on: 18 Apr 2018
 *      Author: zal
 */

#ifndef PUSHABLEOBJECT_H_
#define PUSHABLEOBJECT_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace od
{

    class PushableObject : public RflClass
    {
    public:

        PushableObject();

        virtual void probeFields(FieldProbe &probe) override;
        virtual void onSpawned(LevelObject &obj) override;
        virtual void onDespawned(LevelObject &obj) override;


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


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x0010, "Ground Object", "Pushable Object", PushableObject);

}


#endif /* PUSHABLEOBJECT_H_ */
