/*
 * Key.h
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_KEY_H_
#define INCLUDE_RFL_DRAGON_KEY_H_

#include <dragonRfl/common/ItemCommon.h>

namespace odRfl
{

	class Key : public ItemCommon
	{
	public:

		Key();

		virtual void probeFields(RflFieldProbe &probe) override;


	private:

		RflInteger mLockCode;

	};

}

#endif /* INCLUDE_RFL_DRAGON_KEY_H_ */
