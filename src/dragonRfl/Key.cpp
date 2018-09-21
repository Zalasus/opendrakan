/*
 * Key.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include <dragonRfl/Key.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>

namespace od
{

	Key::Key()
	: mLockCode(0)
	{
	}

	void Key::probeFields(FieldProbe &probe)
    {
        ItemCommon::probeFields(probe);

        probe.beginCategory("Key");
        probe.registerField(mLockCode, "Lock Code");
    }


	OD_REGISTER_RFLCLASS(DragonRfl, Key);

}
