/*
 * RflKey.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include "rfl/dragon/RflKey.h"

namespace od
{

	RflKey::RflKey(RflFieldProbe &probe)
	: RflAbstractItem(probe)
	, mLockCode(probe, "Lock Code", 0)
	{
	}

}
