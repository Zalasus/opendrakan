/*
 * Key.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_KEY_H_
#define INCLUDE_RFL_DRAGON_KEY_H_

#include <dragonRfl/common/ItemCommon.h>

namespace od
{

	class Key : public ItemCommon
	{
	public:

		Key();

		virtual void probeFields(FieldProbe &probe) override;


	private:

		RflInteger mLockCode;

	};


    OD_DECLARE_RFLCLASS_TRAITS(DragonRfl, 0x0021, "Item", "Key", Key);

}

#endif /* INCLUDE_RFL_DRAGON_KEY_H_ */
