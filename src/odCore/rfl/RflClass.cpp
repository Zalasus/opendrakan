/*
 * RflClass.cpp
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#include <odCore/rfl/RflClass.h>

#include <odCore/LevelObject.h>

namespace od
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

	void RflClass::onUpdate(od::LevelObject &obj, double simTime, double relTime)
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



	RflClassRegistrar::RflClassRegistrar(RflClassId classId, const std::string &className)
    : mClassId(classId)
    , mClassName(className)
    {
        Rfl &rfl = Rfl::getSingleton();

        if(rfl.mRegistrarMap.find(classId) != rfl.mRegistrarMap.end())
        {
            Logger::warn() << "Ignoring double registration of RFL class type " << std::hex << classId << std::dec;

        }else
        {
            rfl.mRegistrarMap.insert(std::pair<RflClassId, std::reference_wrapper<RflClassRegistrar>>(classId, *this));
        }
    }

    RflClassRegistrar::~RflClassRegistrar()
    {
    }

    RflClassRegistrar &Rfl::getClassRegistrarById(RflClassId id)
    {
        auto it = mRegistrarMap.find(id);
        if(it == mRegistrarMap.end())
        {
            throw od::NotFoundException("Given class ID is not registered in RFL");
        }

        return it->second;
    }

}
