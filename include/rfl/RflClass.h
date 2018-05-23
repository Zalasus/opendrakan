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
	class LevelObject;
}

namespace odRfl
{

	class RflClass
	{
	public:

		virtual ~RflClass() = default;

		virtual void probeFields(RflFieldProbe &probe) = 0;

		// TODO: maybe create an "EventHandler" interface for this?
		virtual void spawned(od::LevelObject &obj); // does nothing by default
		virtual void despawned(od::LevelObject &obj);
	};

}

#endif /* INCLUDE_RFL_RFLCLASSTYPE_H_ */
