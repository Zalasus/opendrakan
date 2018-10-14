/*
 * ItemCommon.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_COMMON_ITEMCOMMON_H_
#define INCLUDE_RFL_COMMON_ITEMCOMMON_H_

#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>
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

    typedef odRfl::EnumImpl<PlayerSlot, 0, 8> EnumPlayerSlot;



	class ItemCommon : public odRfl::RflClass
	{
	public:

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	protected:

        ItemCommon();


	private:

		odRfl::String        mDisplayName;
		odRfl::Integer       mQuantity;
		odRfl::EnumYesNo     mGrouped;
		odRfl::Integer       mBitmapNumber;
		odRfl::Integer       mIconIndex;
		odRfl::Integer       mIconSlotSize;
		EnumPlayerSlot       mPlayerSlot;
		odRfl::ClassRef		 mPowerupObject;
		odRfl::SoundRef		 mActivateSound;
		odRfl::SoundRef		 mDroppedSound;
		odRfl::SoundRef		 mPickedUpSound;
		odRfl::EnumYesNo	 mDroppable;
		odRfl::Float		 mFadeTime;
	};

}

#endif /* INCLUDE_RFL_COMMON_ITEMCOMMON_H_ */
