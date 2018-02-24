/*
 * RflHealthPack.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include "rfl/dragon/RflHealthPack.h"

#include "rfl/Rfl.h"

namespace od
{

	RflHealthPack::RflHealthPack()
	: mHealthBoost(0)
	{
	}

	void RflHealthPack::probeFields(RflFieldProbe &probe)
	{
	    RflAbstractItem::probeFields(probe);

	    probe.beginCategory("Health Pack");
	    probe.registerField(mHealthBoost, "Health Boost");
	}

    OD_REGISTER_RFL_CLASS(0x000A, "Health Pack", RflHealthPack);

}
