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

namespace od
{

    class DragonRfl;

	class Material : public RflClass
	{
	public:

		Material(DragonRfl &rfl);

		virtual void probeFields(FieldProbe &probe) override;


	private:

		RflSoundRefArray 	mRynnFootSounds;
		RflSoundRefArray	mDragonFootSounds;
		RflSoundRefArray	mWalkerFootSounds;
		RflSoundRefArray	mGiantFootSounds;
		RflSoundRefArray	mKnightFootSounds;
		RflSoundRefArray	mWeaponHitSounds;
		RflFloat			mWeaponFreqShiftRange;
		RflFloat			mMaterialDensity;
		RflClassRef			mWaterEffectProperties;
		RflFloat			mDamagePerSec;
		RflFloat			mFlammability;
		RflSoundRef			mLavaSizzleSound;
		RflClassRef	 		mBurnEffect;
		RflEnumYesNo 		mLandable;
		RflTextureRef		mDetailTexture;
		RflFloat			mDetailScaling;

	};


	OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x004b, "Material", "Standard Material", Material);


	class BlendedMaterial : public Material
	{
	public:

		BlendedMaterial(DragonRfl &rfl);

		virtual void probeFields(FieldProbe &probe) override;


	private:

		RflTextureRef 	mFirstTexture;
		RflTextureRef 	mSecondTexture;
		RflInteger	  	mDirectionOfFlow;
		RflFloat		mFirstSpeed;
		RflFloat		mSecondSpeed;
	};


    OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x0072, "Material", "Blended Material", BlendedMaterial);


}

#endif /* INCLUDE_RFL_DRAGON_MATERIAL_H_ */
