/*
 * Key.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include <dragonRfl/classes/Key.h>

#include <odCore/rfl/Rfl.h>

#include <dragonRfl/RflDragon.h>

namespace dragonRfl
{

	Key::Key()
	: mLockCode(0)
	{
	}

	void Key::probeFields(odRfl::FieldProbe &probe)
    {
        ItemCommon::probeFields(probe);

        probe.beginCategory("Key");
        probe.registerField(mLockCode, "Lock Code");
    }

}
