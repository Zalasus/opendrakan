/*
 * Building.h
 *
 *  Created on: 3 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_BUILDING_H_
#define INCLUDE_RFL_DRAGON_BUILDING_H_

#include <odCore/rfl/DefaultObjectClass.h>

#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace odRender
{
    class Handle;
}

namespace odPhysics
{
    class ObjectHandle;
}

namespace dragonRfl
{

    class BuildingFields : public odRfl::FieldBundle
    {
    public:

        virtual void probeFields(odRfl::FieldProbe &probe) override final;


    protected:

        odRfl::Integer          mInitialHealth;
        odRfl::Enum             mSnapMode;
        odRfl::ClassRef         mSoundEffectObject;
        odRfl::EnumYesNo        mIsDoorWay;
        odRfl::EnumYesNo        mCanArokhLandOn;
        odRfl::Enum             mDoorWayAlong;
        odRfl::EnumMessage      mMessageToSend;
        odRfl::ClassRef         m2DExplosionObject;
        odRfl::ClassRef         mExplosionGenerator;
        odRfl::ClassRef         mSmokeGenerator;
        odRfl::ModelRefArray    mRubbleAnimation;
        odRfl::Float            mRubbleAnimRate;
        odRfl::EnumYesNo        mFlammable;
        odRfl::Enum             mPushOverMode;
        odRfl::EnumYesNo        mFallWhenDead;
        odRfl::SoundRef         mHitGroundSound;
        odRfl::EnumYesNo        mSendMessageWhenPushed;
        odRfl::EnumMessage      mMessageToSendWhenPushed;
        odRfl::EnumYesNo        mSendMessageAfterPushed;
        odRfl::EnumMessage      mMessageToSendAfterPushed;

    };


	class Building : public odRfl::ClassBaseWrapper<Building>, protected odRfl::Spawnable, protected BuildingFields
	{
	};


	class Building_Sv final : public odRfl::ServerClassImpl, private Building
	{
	public:

	    virtual void onSpawned() override;
	    virtual void onDespawned() override;


	private:

	    od::RefPtr<odPhysics::ObjectHandle> mPhysicsHandle;

	};


	class Building_Cl final: public odRfl::ClientClassImpl, private Building
    {
    public:

        Building_Cl(od::Client &client);

        virtual void onSpawned() override;
        virtual void onDespawned() override;


    private:

        od::RefPtr<odPhysics::ObjectHandle> mPhysicsHandle;
        od::RefPtr<odRender::Handle> mRenderHandle;

    };


}

ODRFL_DEFINE_CLASS(dragonRfl::Building, 0x0011, "Ground Object", "Building");
ODRFL_DEFINE_CLASS_FIELDS(dragonRfl::Building, dragonRfl::BuildingFields);

#endif /* INCLUDE_RFL_DRAGON_BUILDING_H_ */
