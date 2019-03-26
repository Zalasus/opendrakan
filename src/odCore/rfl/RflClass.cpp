/*
 * RflClass.cpp
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#include <odCore/rfl/RflClass.h>

#include <odCore/LevelObject.h>

namespace odRfl
{

    void RflClass::onLoaded(od::LevelObject &obj)
    {
    }

    void RflClass::onLoaded(od::Engine &e)
    {
    }

	void RflClass::onSpawned(od::LevelObject &obj)
	{
	}

	void RflClass::onDespawned(od::LevelObject &obj)
	{
	}

	void RflClass::onUpdate(od::LevelObject &obj, float relTime)
	{
	}

	void RflClass::onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message)
	{
	}

	void RflClass::onMoved(od::LevelObject &obj)
	{
	}

	void RflClass::onDestroyed(od::LevelObject &obj)
	{
	}

	bool RflClass::onCreatePhysicsHandles(od::LevelObject &obj, odPhysics::PhysicsSystem &ps)
	{
	    return true;
	}

}


