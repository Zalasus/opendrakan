/*
 * Sky.h
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_SKY_H_
#define INCLUDE_RFL_DRAGON_SKY_H_

#include <osg/Vec3>
#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace odRfl
{

    class DomedSky : public RflClass
    {
    public:

        DomedSky();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void onSpawned(od::LevelObject &obj) override;


    protected:

        RflEnumYesNo	mPrimarySky;
		RflEnum			mFollowMode;
		RflFloat		mOffsetDown;
		RflEnum			mEffects;
		RflClassRef		mLightningObject;
		RflFloat		mAveLightningPeriod;
		RflInteger		mLightningHeight;
		RflInteger		mMinLightningDist;
		RflInteger		mMaxLightningDist;
		RflFloat		mLightningWedgeAngle;
		RflClassRef		mLensFlare;
		RflInteger		mFlareElevation;
		RflInteger		mFlareDirection;

    };

}

#endif /* INCLUDE_RFL_DRAGON_SKY_H_ */
