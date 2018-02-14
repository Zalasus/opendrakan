/*
 * RflClass.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLCLASS_H_
#define INCLUDE_RFL_RFLCLASS_H_

#include "rfl/RflFieldProbe.h"

namespace od
{

	class RflClass
	{
	public:

		RflClass(RflFieldProbe &probe);

		virtual ~RflClass() = default;

	};

}

#endif /* INCLUDE_RFL_RFLCLASSTYPE_H_ */
