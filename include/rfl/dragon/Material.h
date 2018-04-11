/*
 * Material.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_MATERIAL_H_
#define INCLUDE_RFL_DRAGON_MATERIAL_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"

namespace odRfl
{

	class Material : public RflClass
	{
	public:

		Material();

		virtual void probeFields(RflFieldProbe &probe) override;


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


	class BlendedMaterial : public Material
	{
	public:

		BlendedMaterial();

		virtual void probeFields(RflFieldProbe &probe) override;


	private:

		RflTextureRef 	mFirstTexture;
		RflTextureRef 	mSecondTexture;
		RflInteger	  	mDirectionOfFlow;
		RflFloat		mFirstSpeed;
		RflFloat		mSecondSpeed;
	};

}

#endif /* INCLUDE_RFL_DRAGON_MATERIAL_H_ */
