/*
 * RflHealthPack.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include "rfl/dragon/RflHealthPack.h"

namespace od
{

	RflHealthPack::RflHealthPack(RflFieldProbe &probe)
	: RflAbstractItem(probe)
	, mHealthBoost(probe, "Health Boost", 0)
	{
	}

}
