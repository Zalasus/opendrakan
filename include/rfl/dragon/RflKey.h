/*
 * RflKey.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLKEY_H_
#define INCLUDE_RFL_RFLKEY_H_

#include "rfl/general/RflAbstractItem.h"

namespace od
{

	class RflKey : public RflAbstractItem
	{
	public:

		RflKey(RflFieldProbe &probe);


	private:

		RflInteger mLockCode;

	};

}

#endif /* INCLUDE_RFL_RFLKEY_H_ */
