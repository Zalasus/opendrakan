/*
 * HealthPack.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_HEALTHPACK_H_
#define INCLUDE_RFL_DRAGON_HEALTHPACK_H_

#include <dragonRfl/common/ItemCommon.h>

namespace od
{

	class HealthPack : public ItemCommon
	{
	public:

		HealthPack();

		virtual void probeFields(FieldProbe &probe) override;


	private:

		RflInteger mHealthBoost;

	};


	OD_DEFINE_RFLCLASS_TRAITS(DragonRfl, 0x000A, "Item", "Health Pack", HealthPack);

}

#endif /* INCLUDE_RFL_DRAGON_HEALTHPACK_H_ */
