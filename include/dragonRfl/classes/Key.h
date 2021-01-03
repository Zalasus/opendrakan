/*
 * Key.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_KEY_H_
#define INCLUDE_RFL_DRAGON_KEY_H_

#include <dragonRfl/classes/common/ItemCommon.h>

namespace dragonRfl
{

	class Key : public ItemCommon
	{
	public:

		Key();

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::Integer mLockCode;

	};

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::Key, 0x0021, "Item", "Key");

#endif /* INCLUDE_RFL_DRAGON_KEY_H_ */
