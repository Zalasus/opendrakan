/*
 * RflClass.cpp
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#include "rfl/RflClass.h"

#include "LevelObject.h"

namespace odRfl
{


    void RflClass::loaded(od::Engine &e, od::LevelObject *obj)
    {
    }

	void RflClass::spawned(od::LevelObject &obj)
	{
	}

	void RflClass::despawned(od::LevelObject &obj)
	{
	}

	void RflClass::update(od::LevelObject &obj, double simTime, double relTime)
	{
	}

	void RflClass::messageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message)
	{
	}

	void RflClass::onMoved(od::LevelObject &obj)
	{
	}

	void RflClass::destroyed(od::LevelObject &obj)
	{
	}

}
