/*
 * Sky.h
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_SKY_H_
#define INCLUDE_RFL_DRAGON_SKY_H_

#include <odCore/RefCounted.h>

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

#include <odCore/render/ObjectNode.h>

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

        odRfl::EnumYesNo	mPrimarySky;
		odRfl::Enum			mFollowMode;
		odRfl::Float		mOffsetDown;
		odRfl::Enum			mEffects;
		odRfl::ClassRef		mLightningObject;
		odRfl::Float		mAveLightningPeriod;
		odRfl::Integer		mLightningHeight;
		odRfl::Integer		mMinLightningDist;
		odRfl::Integer		mMaxLightningDist;
		odRfl::Float		mLightningWedgeAngle;
		odRfl::ClassRef		mLensFlare;
		odRfl::Integer		mFlareElevation;
		odRfl::Integer		mFlareDirection;


    private:

		od::RefPtr<odRender::ObjectNode> mRenderNode;

    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x001a, "System", "Domed Sky", dragonRfl::DomedSky);

#endif /* INCLUDE_RFL_DRAGON_SKY_H_ */
