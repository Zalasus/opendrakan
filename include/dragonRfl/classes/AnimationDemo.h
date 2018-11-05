/*
 * AnimationDemo.h
 *
 *  Created on: 15 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_
#define INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace dragonRfl
{

    class DragonRfl;

    class AnimationDemo : public odRfl::RflClass
    {
    public:

        AnimationDemo(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onSpawned(od::LevelObject &obj) override;
        virtual void onUpdate(od::LevelObject &obj, double simTime, double relTime) override;


    protected:

        odRfl::AnimRefArray mAnimations;
        odRfl::Float        mSwitchPeriodSeconds;


    private:

        bool mFirstUpdate;
        double mRunningTime;
        size_t mCurrentAnimIndex;

    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x000c, "Debug", "Animation Demo", dragonRfl::AnimationDemo);

#endif /* INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_ */
