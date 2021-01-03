/*
 * PushableObject.h
 *
 *  Created on: 18 Apr 2018
 *      Author: zal
 */

#ifndef PUSHABLEOBJECT_H_
#define PUSHABLEOBJECT_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace dragonRfl
{

    class PushableObject : public odRfl::LevelObjectClassBase
    {
    public:

        PushableObject();

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onSpawned() override;
        virtual void onDespawned() override;


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

ODRFL_DEFINE_CLASS_BASE(dragonRfl::PushableObject, 0x0010, "Ground Object", "Pushable Object");


#endif /* PUSHABLEOBJECT_H_ */
