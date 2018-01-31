/*
 * RflAbstractItem.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLABSTRACTITEM_H_
#define INCLUDE_RFL_RFLABSTRACTITEM_H_

#include <string>

#include "rfl/RflField.h"
#include "rfl/RflClass.h"

namespace od
{

	class RflAbstractItem : public RflClass
	{
	public:

		RflAbstractItem(RflFieldProbe *probe)
		: RflClass(probe)
		, mDisplayName(probe, "DisplayName", "")
		, mQuantity(probe, "Quantity", 1)
		{

		}


	private:

		RflString mDisplayName;
		RflInteger mQuantity;
	};

}

#endif /* INCLUDE_RFL_RFLABSTRACTITEM_H_ */
