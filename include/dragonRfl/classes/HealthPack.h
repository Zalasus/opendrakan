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

		HealthPack();

		virtual void probeFields(odRfl::FieldProbe &probe) override;


	private:

		odRfl::Integer mHealthBoost;

	};

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::HealthPack, 0x000A, "Item", "Health Pack");

#endif /* INCLUDE_RFL_DRAGON_HEALTHPACK_H_ */
