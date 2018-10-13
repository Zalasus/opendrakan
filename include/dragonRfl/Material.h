/*
 * Material.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_MATERIAL_H_
#define INCLUDE_RFL_DRAGON_MATERIAL_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace dragonRfl
{

    class DragonRfl;

	class Material : public odRfl::RflClass
	{
	public:

		Material(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::RflSoundRefArray 	mRynnFootSounds;
		odRfl::RflSoundRefArray	mDragonFootSounds;
		odRfl::RflSoundRefArray	mWalkerFootSounds;
		odRfl::RflSoundRefArray	mGiantFootSounds;
		odRfl::RflSoundRefArray	mKnightFootSounds;
		odRfl::RflSoundRefArray	mWeaponHitSounds;
		odRfl::RflFloat			mWeaponFreqShiftRange;
		odRfl::RflFloat			mMaterialDensity;
		odRfl::RflClassRef			mWaterEffectProperties;
		odRfl::RflFloat			mDamagePerSec;
		odRfl::RflFloat			mFlammability;
		odRfl::RflSoundRef			mLavaSizzleSound;
		odRfl::RflClassRef	 		mBurnEffect;
		odRfl::RflEnumYesNo 		mLandable;
		odRfl::RflTextureRef		mDetailTexture;
		odRfl::RflFloat			mDetailScaling;

	};


	class BlendedMaterial : public Material
	{
	public:

		BlendedMaterial(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::RflTextureRef 	mFirstTexture;
		odRfl::RflTextureRef 	mSecondTexture;
		odRfl::RflInteger	  	mDirectionOfFlow;
		odRfl::RflFloat		    mFirstSpeed;
		odRfl::RflFloat		    mSecondSpeed;
	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x004b, "Material", "Standard Material", dragonRfl::Material);
OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0072, "Material", "Blended Material", dragonRfl::BlendedMaterial);

#endif /* INCLUDE_RFL_DRAGON_MATERIAL_H_ */
