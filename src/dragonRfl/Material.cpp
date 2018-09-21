/*
 * Material.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/Material.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>

namespace od
{

	Material::Material()
	: mRynnFootSounds({})
	, mDragonFootSounds({})
	, mWalkerFootSounds({})
	, mGiantFootSounds({})
	, mKnightFootSounds({})
	, mWeaponHitSounds({})
	, mWeaponFreqShiftRange(0)
	, mMaterialDensity(0.5)
	, mWaterEffectProperties(od::AssetRef::NULL_REF)
	, mDamagePerSec(0)
	, mFlammability(1)
	, mLavaSizzleSound(od::AssetRef::NULL_REF)
	, mBurnEffect(od::AssetRef::NULL_REF)
	, mLandable(true)
	, mDetailTexture(od::AssetRef::NULL_REF)
	, mDetailScaling(0.25)
	{
	}

	void Material::probeFields(FieldProbe &probe)
	{
	    probe.beginCategory("Solid Ground Sounds");
	    probe.registerField(mRynnFootSounds, "Rynn Foot Sounds");
	    probe.registerField(mDragonFootSounds, "Dragon Foot Sounds");
	    probe.registerField(mWalkerFootSounds, "Walker Foot Sounds");
	    probe.registerField(mGiantFootSounds, "Giant Foot Sounds");
	    probe.registerField(mKnightFootSounds, "Knight Foot Sounds");
	    probe.registerField(mWeaponHitSounds, "Weapon Hit Sounds");
	    probe.registerField(mWeaponFreqShiftRange, "Weapon Freq Shift Range (0.0 - 24.0 notes)");

	    probe.beginCategory("Material Properties");
	    probe.registerField(mMaterialDensity, "Material Density (0 soft - 1 hard)");
	    probe.registerField(mWaterEffectProperties, "Water Effect Properties");
	    probe.registerField(mDamagePerSec, "Damage (hp/s)");
	    probe.registerField(mFlammability, "Flammability (from 0 and up)");
	    probe.registerField(mLavaSizzleSound, "Lava Sizzle Sound");
	    probe.registerField(mBurnEffect, "Burn Effect");
	    probe.registerField(mLandable, "Landable?");

	    probe.beginCategory("Multi-Texturing Effects");
	    probe.registerField(mDetailTexture, "Detail Texture");
	    probe.registerField(mDetailScaling, "Detail Scaling");
	}


    OD_REGISTER_RFLCLASS(DragonRfl, Material);



	BlendedMaterial::BlendedMaterial()
	: mFirstTexture(od::AssetRef::NULL_REF)
	, mSecondTexture(od::AssetRef::NULL_REF)
	, mDirectionOfFlow(0)
	, mFirstSpeed(0.1)
	, mSecondSpeed(-0.1)
	{
	}

	void BlendedMaterial::probeFields(FieldProbe &probe)
    {
	    Material::probeFields(probe);

	    probe.beginCategory("Blended Material");
        probe.registerField(mFirstTexture, "First Texture");
        probe.registerField(mSecondTexture, "Second Texture");
        probe.registerField(mDirectionOfFlow, "Direction of Flow (0-360)");
        probe.registerField(mFirstSpeed, "First Speed (lu/s)");
        probe.registerField(mSecondSpeed, "Second Speed (lu/s)");
    }


    OD_REGISTER_RFLCLASS(DragonRfl, BlendedMaterial);

}
