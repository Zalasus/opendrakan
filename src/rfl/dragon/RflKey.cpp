/*
 * RflKey.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include "rfl/dragon/RflKey.h"

#include "rfl/Rfl.h"

namespace od
{

	RflKey::RflKey()
	: mLockCode(0)
	{
	}

	void RflKey::probeFields(RflFieldProbe &probe)
    {
        RflAbstractItem::probeFields(probe);

        probe.beginCategory("Key");
        probe.registerField(mLockCode, "Lock Code");
    }

	OD_REGISTER_RFL_CLASS(0x0021, "Key", RflKey);
}
