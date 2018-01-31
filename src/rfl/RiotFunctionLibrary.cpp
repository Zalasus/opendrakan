/*
 * RiotFunctionLibrary.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include "rfl/RiotFunctionLibrary.h"

#include "Logger.h"
#include "Exception.h"

namespace od
{

	RflClassRegistrar::RflClassRegistrar(RflClassId classId, const std::string &className)
	: mClassId(classId)
    , mClassName(className)
	{
		RiotFunctionLibrary &rfl = RiotFunctionLibrary::getSingleton();

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



	RiotFunctionLibrary::RiotFunctionLibrary(const std::string &name)
	: mName(name)
	{
	}

	RflClassRegistrar &RiotFunctionLibrary::getClassRegistrarById(RflClassId id)
	{
	    auto it = mRegistrarMap.find(id);
		if(it != mRegistrarMap.end())
		{
			throw Exception("Given class ID is not registered in RFL");
		}

		return it->second;
	}
}



