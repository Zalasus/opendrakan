/*
 * Key.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_KEY_H_
#define INCLUDE_RFL_DRAGON_KEY_H_

#include <dragonRfl/common/ItemCommon.h>

namespace dragonRfl
{

    class DragonRfl;

	class Key : public ItemCommon
	{
	public:

		Key(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::RflInteger mLockCode;

	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0021, "Item", "Key", dragonRfl::Key);

#endif /* INCLUDE_RFL_DRAGON_KEY_H_ */
