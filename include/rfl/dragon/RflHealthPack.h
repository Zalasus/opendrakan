/*
 * RflHealthPack.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLHEALTHPACK_H_
#define INCLUDE_RFL_RFLHEALTHPACK_H_

#include "rfl/general/RflAbstractItem.h"

namespace od
{

	class RflHealthPack : public RflAbstractItem
	{
	public:

		RflHealthPack();

		virtual void probeFields(RflFieldProbe &probe) override;


	private:

		RflInteger mHealthBoost;

	};

}

#endif /* INCLUDE_RFL_RFLHEALTHPACK_H_ */
