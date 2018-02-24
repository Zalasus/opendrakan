/*
 * RiotFunctionLibrary.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <rfl/Rfl.h>
#include "Logger.h"
#include "Exception.h"

namespace od
{

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



	Rfl::RflNameSetter::RflNameSetter(const std::string &name)
	{
		Rfl::getSingleton().setName(name);
	}

	RflClassRegistrar &Rfl::getClassRegistrarById(RflClassId id)
	{
	    auto it = mRegistrarMap.find(id);
		if(it == mRegistrarMap.end())
		{
			throw NotFoundException("Given class ID is not registered in RFL");
		}

		return it->second;
	}

	Rfl &Rfl::getSingleton()
	{
		static Rfl rfl;

		return rfl;
	}
}




