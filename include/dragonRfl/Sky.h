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

namespace od
{

    class DomedSky : public RflClass
    {
    public:

        DomedSky();

        virtual void probeFields(FieldProbe &probe) override;
        virtual void onSpawned(LevelObject &obj) override;


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


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x001a, "System", "Domed Sky", DomedSky);

}

#endif /* INCLUDE_RFL_DRAGON_SKY_H_ */
