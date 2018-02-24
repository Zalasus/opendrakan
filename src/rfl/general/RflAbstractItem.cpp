/*
 * RflAbstractItem.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */


#include "rfl/general/RflAbstractItem.h"

namespace od
{

	RflAbstractItem::RflAbstractItem()
	: mDisplayName("")
	, mQuantity(1)
	, mGrouped(0)
	, mBitmapNumber(1)
	, mIconIndex(1)
	, mIconSlotSize(1)
	, mPlayerSlot(0)
	, mPowerupObject(AssetRef::NULL_REF)
	, mActivateSound(AssetRef::NULL_REF)
	, mDroppedSound(AssetRef::NULL_REF)
	, mPickedUpSound(AssetRef::NULL_REF)
	, mDroppable(0)
	, mFadeTime(60)
	{
	}

	void RflAbstractItem::probeFields(RflFieldProbe &probe)
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
