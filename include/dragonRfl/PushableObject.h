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

namespace dragonRfl
{

    class DragonRfl;

    class PushableObject : public odRfl::RflClass
    {
    public:

        PushableObject(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onDespawned(od::LevelObject &obj) override;


    protected:

        odRfl::RflEnumYesNo       mWaitForTrigger;
        odRfl::RflEnumYesNo       mBurnable;
        odRfl::RflSoundRefArray   mBounceSounds;
        odRfl::RflSoundRef        mLoopedRollSound;
        odRfl::RflEnumYesNo       mDetectOtherPushables;
        odRfl::RflClassRef        mSpecialEffect;
        odRfl::RflEnum            mWaterEffects;
        odRfl::RflEnumYesNo       mGory;
        odRfl::RflEnum            mPushMode;
        odRfl::RflEnum            mPivotPoint;
        odRfl::RflFloat           mFriction;
        odRfl::RflFloat           mElasticity;
        odRfl::RflEnumYesNo       mRigidBody;
        odRfl::RflFloat           mLifeTime;
        odRfl::RflInteger         mHealth;
        odRfl::RflClassRef        mExplosionGenerator;
        odRfl::RflInteger         mDamage;
        odRfl::RflEnum            mDamagePlayer;

    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0010, "Ground Object", "Pushable Object", dragonRfl::PushableObject);


#endif /* PUSHABLEOBJECT_H_ */
