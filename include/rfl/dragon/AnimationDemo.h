/*
 * AnimationDemo.h
 *
 *  Created on: 15 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_
#define INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"
#include "anim/SkeletonAnimationPlayer.h"

namespace odRfl
{

    class AnimationDemo : public RflClass
    {
    public:

        AnimationDemo();

        virtual void probeFields(RflFieldProbe &probe) override;

        virtual void loaded(od::Engine &engine, od::LevelObject *obj) override;
        virtual void update(od::LevelObject &obj, double simTime, double relTime) override;


    protected:

        RflAnimRefArray mAnimations;
        RflFloat        mSwitchPeriodSeconds;


    private:

        bool mFirstUpdate;
        double mRunningTime;
        size_t mCurrentAnimIndex;
        osg::ref_ptr<od::SkeletonAnimationPlayer> mAnimationPlayer;

    };

}

#endif /* INCLUDE_RFL_DRAGON_ANIMATIONDEMO_H_ */
