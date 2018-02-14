/*
 * RflAbstractItem.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLABSTRACTITEM_H_
#define INCLUDE_RFL_RFLABSTRACTITEM_H_

#include <string>

#include "rfl/RflField.h"
#include "rfl/RflClass.h"

namespace od
{

	class RflAbstractItem : public RflClass
	{
	public:

		RflAbstractItem(RflFieldProbe &probe)
		: RflClass(probe)
		, mDisplayName(probe, "DisplayName", "")
		, mQuantity(probe, "Quantity", 1)
		, mGrouped(probe, "Grouped", 0)
		, mBitmapNumber(probe, "Bit Map Number", 1)
		, mIconIndex(probe, "Icon Index (1-8)", 1)
		, mIconSlotSize(probe, "Icon Slot Size (1-3)", 1)
		, mPlayerSlot(probe, "Player Slot", 0)
		, mPowerupObject(probe, "Powerup Object", AssetRef::NULL_REF)
		, mActivateSound(probe, "Activate Sound", AssetRef::NULL_REF)
		, mDroppedSound(probe, "Dropped Sound", AssetRef::NULL_REF)
		, mPickedUpSound(probe, "Picked Up Sound", AssetRef::NULL_REF)
		, mDroppable(probe, "Droppable?", 0)
		, mFadeTime(probe, "Fade Time when dropped (Sec)", 60)
		{
		}


	private:

		RflString         mDisplayName;
		RflInteger        mQuantity;
		RflEnumYesNo      mGrouped;
		RflInteger        mBitmapNumber;
		RflInteger        mIconIndex;
		RflInteger        mIconSlotSize;
		RflEnumPlayerSlot mPlayerSlot;
		RflClassRef		  mPowerupObject;
		RflSoundRef		  mActivateSound;
		RflSoundRef		  mDroppedSound;
		RflSoundRef		  mPickedUpSound;
		RflEnumYesNo	  mDroppable;
		RflFloat		  mFadeTime;
	};

}

#endif /* INCLUDE_RFL_RFLABSTRACTITEM_H_ */
