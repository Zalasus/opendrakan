/*
 * RflAbstractItem.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLABSTRACTITEM_H_
#define INCLUDE_RFL_RFLABSTRACTITEM_H_

#include <string>

#include "RflClassType.h"
#include "RflField.h"

namespace od
{

	class RflAbstractItem : public RflClassType
	{
	public:

		RflAbstractItem(RflClassBuilder &cb)
		: RflClassType(cb)
		, mDisplayName(cb, "DisplayName", "")
		, mQuantity(cb, "Quantity", 1)
		{

		}


	private:

		RflString mDisplayName;
		RflInteger mQuantity;
	};

}

#endif /* INCLUDE_RFL_RFLABSTRACTITEM_H_ */
