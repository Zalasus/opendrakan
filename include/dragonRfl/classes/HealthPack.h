/*
 * HealthPack.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_HEALTHPACK_H_
#define INCLUDE_RFL_DRAGON_HEALTHPACK_H_

#include <dragonRfl/classes/common/ItemCommon.h>

namespace dragonRfl
{

    class DragonRfl;

	class HealthPack : public ItemCommon
	{
	public:

		HealthPack(DragonRfl &rfl);

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::Integer mHealthBoost;

	};

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x000A, "Item", "Health Pack", dragonRfl::HealthPack);

#endif /* INCLUDE_RFL_DRAGON_HEALTHPACK_H_ */
