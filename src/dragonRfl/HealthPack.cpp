/*
 * HealthPack.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/HealthPack.h>

#include <odCore/rfl/Rfl.h>

namespace odRfl
{

	HealthPack::HealthPack()
	: mHealthBoost(0)
	{
	}

	void HealthPack::probeFields(RflFieldProbe &probe)
	{
	    ItemCommon::probeFields(probe);

	    probe.beginCategory("Health Pack");
	    probe.registerField(mHealthBoost, "Health Boost");
	}

    OD_REGISTER_RFL_CLASS(0x000A, "Health Pack", HealthPack);

}
