/*
 * RflKey.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLKEY_H_
#define INCLUDE_RFL_RFLKEY_H_

#include "rfl/RflAbstractItem.h"
#include "rfl/RiotFunctionLibrary.h"

namespace od
{

	class RflKey : public RflAbstractItem
	{
	public:

		RflKey(RflFieldProbe &probe)
		: RflAbstractItem(probe)
		, mLockCode(probe, "Lock Code", 0)
		{
		}


	private:

		RflInteger mLockCode;

	};

}

#endif /* INCLUDE_RFL_RFLKEY_H_ */
