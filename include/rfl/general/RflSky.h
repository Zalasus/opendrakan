/*
 * RflSky.h
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_GENERAL_RFLSKY_H_
#define INCLUDE_RFL_GENERAL_RFLSKY_H_

#include "rfl/RflClass.h"

namespace od
{

    class RflDomedSky : public RflClass
    {
    public:

        RflDomedSky();

        virtual void probeFields(RflFieldProbe &probe) override;
        virtual void spawn(LevelObject &obj) override;


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

#endif /* INCLUDE_RFL_GENERAL_RFLSKY_H_ */
