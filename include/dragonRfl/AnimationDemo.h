/*
 * AnimationDemo.h
 *
 *  Created on: 15 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_
#define INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>
#include <odCore/anim/SkeletonAnimationPlayer.h>

namespace od
{

    class AnimationDemo : public RflClass
    {
    public:

        AnimationDemo();

        virtual void probeFields(FieldProbe &probe) override;

        virtual void onLoaded(LevelObject &obj) override;
        virtual void onUpdate(LevelObject &obj, double simTime, double relTime) override;


    protected:

        RflAnimRefArray mAnimations;
        RflFloat        mSwitchPeriodSeconds;


    private:

        bool mFirstUpdate;
        double mRunningTime;
        size_t mCurrentAnimIndex;
        osg::ref_ptr<od::SkeletonAnimationPlayer> mAnimationPlayer;

    };

    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x000c, "Debug", "Animation Demo", AnimationDemo);

}

#endif /* INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_ */
