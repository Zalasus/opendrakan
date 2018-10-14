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

namespace dragonRfl
{

    class DragonRfl;

    class DomedSky : public odRfl::RflClass
    {
    public:

        DomedSky(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onSpawned(od::LevelObject &obj) override;


    protected:

        odRfl::RflEnumYesNo	mPrimarySky;
		odRfl::RflEnum			mFollowMode;
		odRfl::RflFloat		mOffsetDown;
		odRfl::RflEnum			mEffects;
		odRfl::RflClassRef		mLightningObject;
		odRfl::RflFloat		mAveLightningPeriod;
		odRfl::RflInteger		mLightningHeight;
		odRfl::RflInteger		mMinLightningDist;
		odRfl::RflInteger		mMaxLightningDist;
		odRfl::RflFloat		mLightningWedgeAngle;
		odRfl::RflClassRef		mLensFlare;
		odRfl::RflInteger		mFlareElevation;
		odRfl::RflInteger		mFlareDirection;

    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x001a, "System", "Domed Sky", dragonRfl::DomedSky);

#endif /* INCLUDE_RFL_DRAGON_SKY_H_ */
