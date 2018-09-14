/*
 * ItemCommon.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_COMMON_ITEMCOMMON_H_
#define INCLUDE_RFL_COMMON_ITEMCOMMON_H_

#include <odCore/rfl/RflField.h>
#include <odCore/rfl/RflClass.h>

namespace odRfl
{

    enum class PlayerSlot
    {
        None,
        RightHand,
        LeftHand,
        EitherHand,
        Body,
        Head,
        Feet,
        Wrists,
        Any
    };

    typedef RflEnumImpl<PlayerSlot, 0, 8> RflEnumPlayerSlot;



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
