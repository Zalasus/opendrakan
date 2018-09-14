/*
 * HealthPack.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_HEALTHPACK_H_
#define INCLUDE_RFL_DRAGON_HEALTHPACK_H_

#include <dragonRfl/common/ItemCommon.h>

namespace odRfl
{

	class HealthPack : public ItemCommon
	{
	public:

		HealthPack();

		virtual void probeFields(RflFieldProbe &probe) override;


	private:

		RflInteger mHealthBoost;

	};

}

#endif /* INCLUDE_RFL_DRAGON_HEALTHPACK_H_ */
