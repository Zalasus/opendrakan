/*
 * Sky.h
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_SKY_H_
#define INCLUDE_RFL_DRAGON_SKY_H_

#include <memory>

#include <odCore/rfl/Class.h>
#include <odCore/rfl/DummyClass.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

#include <odCore/render/Handle.h>

namespace dragonRfl
{

    struct DomedSkyFields final : public odRfl::FieldBundle
    {
        enum class FollowMode
        {
            ORIGINAL_HEIGHT,
            CAMERA,
            LANDSCAPE,
            HOLD_POSITION
        };

        enum class Effects
        {
            NONE,
            LIGHTNING
        };

        DomedSkyFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::EnumYesNo	              primarySky;
		odRfl::EnumImpl<FollowMode, 0, 3> followMode;
		odRfl::Float		              offsetDown;
		odRfl::EnumImpl<Effects, 0, 1>    effects;
		odRfl::ClassRef		              lightningObject;
		odRfl::Float		              aveLightningPeriod;
		odRfl::Integer		              lightningHeight;
		odRfl::Integer		              minLightningDist;
		odRfl::Integer		              maxLightningDist;
		odRfl::Float		              lightningWedgeAngle;
		odRfl::ClassRef		              lensFlare;
		odRfl::Integer		              flareElevation;
		odRfl::Integer		              flareDirection;
    };


    class DomedSky_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<DomedSky_Cl>
    {
    public:

        DomedSky_Cl();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onSpawned() override;
        virtual void onPostUpdate(float relTime) override;


    private:

        DomedSkyFields mFields;

		std::shared_ptr<odRender::Handle> mRenderNode;

        std::shared_ptr<od::LevelObject> mCameraObject;

    };


    using DomedSkyFactory = odRfl::ClientOnlyClassFactory<DomedSkyFields, DomedSky_Cl>;


    OD_DEFINE_CLASS(DomedSky, 0x001a, "System", "Domed Sky", DomedSkyFactory);

}

#endif /* INCLUDE_RFL_DRAGON_SKY_H_ */
