/*
 * Key.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include "rfl/dragon/Key.h"

#include "rfl/Rfl.h"

namespace odRfl
{

	Key::Key()
	: mLockCode(0)
	{
	}

	void Key::probeFields(RflFieldProbe &probe)
    {
        ItemCommon::probeFields(probe);

        probe.beginCategory("Key");
        probe.registerField(mLockCode, "Lock Code");
    }

	OD_REGISTER_RFL_CLASS(0x0021, "Key", Key);
}
