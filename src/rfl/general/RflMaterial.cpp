/*
 * RflMaterial.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include "rfl/general/RflMaterial.h"

namespace od
{

	RflMaterial::RflMaterial(RflFieldProbe &probe)
	: RflClass(probe)
	, mRynnFootSounds(probe, "Rynn Foot Sounds", {})
	, mDragonFootSounds(probe, "Dragon Foot Sounds", {})
	, mWalkerFootSounds(probe, "Walker Foot Sounds", {})
	, mGiantFootSounds(probe, "Giant Foot Sounds", {})
	, mKnightFootSounds(probe, "Knight Foot Sounds", {})
	, mWeaponHitSounds(probe, "Weapon Hit Sounds", {})
	, mWeaponFreqShiftRange(probe, "Weapon Freq Shift Range (0.0 - 24.0 notes)", 0)
	, mMaterialDensity(probe, "Material Density (0 soft - 1 hard)", 0.5)
	, mWaterEffectProperties(probe, "Water Effect Properties", AssetRef::NULL_REF)
	, mDamagePerSec(probe, "Damage (hp/s)", 0)
	, mFlammability(probe, "Flammability (from 0 and up)", 1)
	, mLavaSizzleSound(probe, "Lava Sizzle Sound", AssetRef::NULL_REF)
	, mBurnEffect(probe, "Burn Effect", AssetRef::NULL_REF)
	, mLandable(probe, "Landable?", 1)
	, mDetailTexture(probe, "Detail Texture", AssetRef::NULL_REF)
	, mDetailScaling(probe, "Detail Scaling", 0.25)
	{
	}


	RflBlendedMaterial::RflBlendedMaterial(RflFieldProbe &probe)
	: RflMaterial(probe)
	, mFirstTexture(probe, "First Texture", AssetRef::NULL_REF)
	, mSecondTexture(probe, "Second Texture", AssetRef::NULL_REF)
	, mDirectionOfFlow(probe, "Direction of Flow (0-360)", 0)
	, mFirstSpeed(probe, "First Speed (lu/s)", 0.1)
	, mSecondSpeed(probe, "Second Speed (lu/s)", -0.1)
	{
	}


}
