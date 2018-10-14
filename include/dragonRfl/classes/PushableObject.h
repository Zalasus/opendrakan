/*
 * PushableObject.h
 *
 *  Created on: 18 Apr 2018
 *      Author: zal
 */

#ifndef PUSHABLEOBJECT_H_
#define PUSHABLEOBJECT_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

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

        odRfl::EnumYesNo       mWaitForTrigger;
        odRfl::EnumYesNo       mBurnable;
        odRfl::SoundRefArray   mBounceSounds;
        odRfl::SoundRef        mLoopedRollSound;
        odRfl::EnumYesNo       mDetectOtherPushables;
        odRfl::ClassRef        mSpecialEffect;
        odRfl::Enum            mWaterEffects;
        odRfl::EnumYesNo       mGory;
        odRfl::Enum            mPushMode;
        odRfl::Enum            mPivotPoint;
        odRfl::Float           mFriction;
        odRfl::Float           mElasticity;
        odRfl::EnumYesNo       mRigidBody;
        odRfl::Float           mLifeTime;
        odRfl::Integer         mHealth;
        odRfl::ClassRef        mExplosionGenerator;
        odRfl::Integer         mDamage;
        odRfl::Enum            mDamagePlayer;

    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0010, "Ground Object", "Pushable Object", dragonRfl::PushableObject);


#endif /* PUSHABLEOBJECT_H_ */
