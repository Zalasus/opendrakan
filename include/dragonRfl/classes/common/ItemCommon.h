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

namespace dragonRfl
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

    typedef odRfl::RflEnumImpl<PlayerSlot, 0, 8> RflEnumPlayerSlot;



	class ItemCommon : public odRfl::RflClass
	{
	public:

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	protected:

        ItemCommon();


	private:

		odRfl::RflString         mDisplayName;
		odRfl::RflInteger        mQuantity;
		odRfl::RflEnumYesNo      mGrouped;
		odRfl::RflInteger        mBitmapNumber;
		odRfl::RflInteger        mIconIndex;
		odRfl::RflInteger        mIconSlotSize;
		RflEnumPlayerSlot        mPlayerSlot;
		odRfl::RflClassRef		 mPowerupObject;
		odRfl::RflSoundRef		 mActivateSound;
		odRfl::RflSoundRef		 mDroppedSound;
		odRfl::RflSoundRef		 mPickedUpSound;
		odRfl::RflEnumYesNo	     mDroppable;
		odRfl::RflFloat		     mFadeTime;
	};

}

#endif /* INCLUDE_RFL_COMMON_ITEMCOMMON_H_ */
