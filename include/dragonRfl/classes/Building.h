/*
 * Building.h
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_BUILDING_H_
#define INCLUDE_RFL_DRAGON_BUILDING_H_

#include <odCore/ObjectLightReceiver.h>

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

#include <odCore/render/Handle.h>

namespace dragonRfl
{

    struct BuildingFields final : public odRfl::FieldBundle
    {
        BuildingFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override final;

        odRfl::Integer          initialHealth;
        odRfl::Enum             snapMode;
        odRfl::ClassRef         soundEffectObject;
        odRfl::EnumYesNo        isDoorWay;
        odRfl::EnumYesNo        canArokhLandOn;
        odRfl::Enum             doorWayAlong;
        odRfl::EnumMessage      messageToSend;
        odRfl::ClassRef         m2DExplosionObject;
        odRfl::ClassRef         explosionGenerator;
        odRfl::ClassRef         smokeGenerator;
        odRfl::ModelRefArray    rubbleAnimation;
        odRfl::Float            rubbleAnimRate;
        odRfl::EnumYesNo        flammable;
        odRfl::Enum             pushOverMode;
        odRfl::EnumYesNo        fallWhenDead;
        odRfl::SoundRef         hitGroundSound;
        odRfl::EnumYesNo        sendMessageWhenPushed;
        odRfl::EnumMessage      messageToSendWhenPushed;
        odRfl::EnumYesNo        sendMessageAfterPushed;
        odRfl::EnumMessage      messageToSendAfterPushed;

    };


	class Building_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<Building_Sv>
	{
	public:

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

	    virtual void onSpawned() override;
	    virtual void onDespawned() override;


	private:

        BuildingFields mFields;

	};


	class Building_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<Building_Cl>
    {
    public:

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onSpawned() override;


    private:

        BuildingFields mFields;

        std::shared_ptr<odRender::Handle> mRenderHandle;

    };

    using BuildingFactory = odRfl::DefaultClassFactory<BuildingFields, Building_Cl, Building_Sv>;

    OD_DEFINE_CLASS(Building, 0x0011, "Ground Object", "Building", BuildingFactory);

}

#endif /* INCLUDE_RFL_DRAGON_BUILDING_H_ */
