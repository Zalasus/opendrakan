/*
 * ItemCommon.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include <dragonRfl/classes/common/ItemCommon.h>

namespace dragonRfl
{

	ItemCommon::ItemCommon()
	: mDisplayName("")
	, mQuantity(1)
	, mGrouped(false)
	, mBitmapNumber(1)
	, mIconIndex(1)
	, mIconSlotSize(1)
	, mPlayerSlot(PlayerSlot::None)
	, mPowerupObject(odDb::AssetRef::NULL_REF)
	, mActivateSound(odDb::AssetRef::NULL_REF)
	, mDroppedSound(odDb::AssetRef::NULL_REF)
	, mPickedUpSound(odDb::AssetRef::NULL_REF)
	, mDroppable(false)
	, mFadeTime(60)
	{
	}

	void ItemCommon::probeFields(odRfl::FieldProbe &probe)
    {
	    probe.beginCategory("Item");
	    probe.registerField(mDisplayName, "DisplayName");
	    probe.registerField(mQuantity, "Quantity");
	    probe.registerField(mGrouped, "Grouped");
        probe.registerField(mBitmapNumber, "Bit Map Number");
        probe.registerField(mIconIndex, "Icon Index (1-8)");
        probe.registerField(mIconSlotSize, "Icon Slot Size (1-3)");
        probe.registerField(mPlayerSlot, "Player Slot");
        probe.registerField(mPowerupObject, "Powerup Object");
        probe.registerField(mActivateSound, "Activate Sound");
        probe.registerField(mDroppedSound, "Dropped Sound");
        probe.registerField(mPickedUpSound, "Picked Up Sound");
        probe.registerField(mDroppable, "Droppable?");
        probe.registerField(mFadeTime, "Fade Time when dropped (Sec)");
    }


}
