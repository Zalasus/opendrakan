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

	RflClassTypeRegistrar::RflClassTypeRegistrar(RflClassTypeId typeId, const std::string &typeName)
	: mTypeId(typeId)
    , mTypeName(typeName)
	{
		RiotFunctionLibrary &rfl = RiotFunctionLibrary::getSingleton();

		if(rfl.mRegistrarMap.find(typeId) != rfl.mRegistrarMap.end())
		{
			Logger::warn() << "Ignoring double registration of RFL class type " << std::hex << typeId << std::dec;

		}else
		{
			rfl.mRegistrarMap[typeId] = this;
		}
	}

	RflClassTypeRegistrar::~RflClassTypeRegistrar()
	{
	}



	RiotFunctionLibrary::RiotFunctionLibrary(const std::string &name)
	: mName(name)
	{
	}

	RflClassType &RiotFunctionLibrary::getClassTypeById(RflClassTypeId id)
	{
		if(mRegistrarMap.find(id) != mRegistrarMap.end())
		{
			throw Exception("Given class type ID is not registered in RFL");
		}

		return mRegistrarMap[id]->getClassType();
	}
}




