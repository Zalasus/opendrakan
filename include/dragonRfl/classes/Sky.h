/*
 * Sky.h
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_SKY_H_
#define INCLUDE_RFL_DRAGON_SKY_H_

#include <odCore/RefCounted.h>

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

#include <odCore/render/Handle.h>

#include <dragonRfl/classes/special/SelfDestruct.h>

namespace dragonRfl
{

    class DragonRfl;

    ODRFL_DEFINE_CLASS(0x001a, "System", "Domed Sky", DomedSky);

    class DomedSkyImpl : public odRfl::LevelObjectClassImpl
    {
    public:

        DomedSkyImpl(DragonRfl &rfl, od::LevelObject &obj);

        virtual void probeFields(odRfl::FieldProbe &probe) override;
        virtual void onSpawned() override;
        virtual void onTranslated(const glm::vec3 &from, const glm::vec3 &to) override;


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

		od::RefPtr<odRender::Handle> mRenderNode;

    };

    OD_IMPLEMENT_RFLCLASS_CLIENT(DomedSky, DomedSkyImpl);
    OD_IMPLEMENT_RFLCLASS_SERVER(DomedSky, SelfDestruct);

}

#endif /* INCLUDE_RFL_DRAGON_SKY_H_ */
