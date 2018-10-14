/*
 * Material.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_MATERIAL_H_
#define INCLUDE_RFL_DRAGON_MATERIAL_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace dragonRfl
{

    class DragonRfl;

	class Material : public odRfl::RflClass
	{
	public:

		Material(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::SoundRefArray 	mRynnFootSounds;
		odRfl::SoundRefArray	mDragonFootSounds;
		odRfl::SoundRefArray	mWalkerFootSounds;
		odRfl::SoundRefArray	mGiantFootSounds;
		odRfl::SoundRefArray	mKnightFootSounds;
		odRfl::SoundRefArray	mWeaponHitSounds;
		odRfl::Float			mWeaponFreqShiftRange;
		odRfl::Float			mMaterialDensity;
		odRfl::ClassRef			mWaterEffectProperties;
		odRfl::Float			mDamagePerSec;
		odRfl::Float			mFlammability;
		odRfl::SoundRef			mLavaSizzleSound;
		odRfl::ClassRef	 		mBurnEffect;
		odRfl::EnumYesNo 		mLandable;
		odRfl::TextureRef		mDetailTexture;
		odRfl::Float			mDetailScaling;

	};


	class BlendedMaterial : public Material
	{
	public:

		BlendedMaterial(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::TextureRef   mFirstTexture;
		odRfl::TextureRef   mSecondTexture;
		odRfl::Integer      mDirectionOfFlow;
		odRfl::Float        mFirstSpeed;
		odRfl::Float        mSecondSpeed;
	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x004b, "Material", "Standard Material", dragonRfl::Material);
OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0072, "Material", "Blended Material", dragonRfl::BlendedMaterial);

#endif /* INCLUDE_RFL_DRAGON_MATERIAL_H_ */
