/*
 * Material.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_MATERIAL_H_
#define INCLUDE_RFL_DRAGON_MATERIAL_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace dragonRfl
{

	class Material : public odRfl::MaterialClassBase
	{
	public:

		Material();

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

		BlendedMaterial();

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::TextureRef   mFirstTexture;
		odRfl::TextureRef   mSecondTexture;
		odRfl::Integer      mDirectionOfFlow;
		odRfl::Float        mFirstSpeed;
		odRfl::Float        mSecondSpeed;
	};

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::Material, 0x004b, "Material", "Standard Material");
ODRFL_DEFINE_CLASS_BASE(dragonRfl::BlendedMaterial, 0x0072, "Material", "Blended Material");

#endif /* INCLUDE_RFL_DRAGON_MATERIAL_H_ */
