/*
 * RflClass.cpp
 *
 *  Created on: 13 Feb 2018
 *      Author: zal
 */

#include "rfl/RflClass.h"

#include "LevelObject.h"

namespace od
{

	RflClass::RflClass()
	: mLevelObject(nullptr)
	{
	}

	void RflClass::setLevelObject(LevelObject &obj)
	{
	    mLevelObject = &obj;
	}

}
