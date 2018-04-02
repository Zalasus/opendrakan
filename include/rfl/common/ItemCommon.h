/*
 * ItemCommon.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_COMMON_ITEMCOMMON_H_
#define INCLUDE_RFL_COMMON_ITEMCOMMON_H_

#include "rfl/RflField.h"
#include "rfl/RflClass.h"

namespace odRfl
{

	class ItemCommon : public RflClass
	{
	public:

		virtual void probeFields(RflFieldProbe &probe) override;


	protected:

        ItemCommon();


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

#endif /* INCLUDE_RFL_COMMON_ITEMCOMMON_H_ */
