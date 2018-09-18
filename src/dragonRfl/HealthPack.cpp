/*
 * HealthPack.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/HealthPack.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>

namespace od
{

	HealthPack::HealthPack()
	: mHealthBoost(0)
	{
	}

	void HealthPack::probeFields(FieldProbe &probe)
	{
	    ItemCommon::probeFields(probe);

	    probe.beginCategory("Health Pack");
	    probe.registerField(mHealthBoost, "Health Boost");
	}

    OD_REGISTER_RFL_CLASS(DragonRfl, 0x000A, "Item", "Health Pack", HealthPack);

}
