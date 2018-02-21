/*
 * RflClass.cpp
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#include "rfl/RflClass.h"

namespace od
{

	RflClass::RflClass(RflFieldProbe &probe)
	: mLevelObject(nullptr)
	{
	}

	void RflClass::setLevelObject(Object &obj)
	{
	    mLevelObject = &obj;
	}

}
