/*
 * RflHealthPack.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLHEALTHPACK_H_
#define INCLUDE_RFL_RFLHEALTHPACK_H_

#include "rfl/RflAbstractItem.h"
#include "rfl/RiotFunctionLibrary.h"

namespace od
{

	class RflHealthPack : public RflAbstractItem
	{
	public:

		RflHealthPack(RflFieldProbe &probe)
		: RflAbstractItem(probe)
		, mHealthBoost(probe, "Health Boost", 0)
		{
		}


	private:

		RflInteger mHealthBoost;

	};

}

#endif /* INCLUDE_RFL_RFLHEALTHPACK_H_ */
