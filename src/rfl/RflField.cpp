/*
 * RflField.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include "rfl/RflFieldProbe.h"
#include "rfl/RflField.h"


namespace od
{

    void RflField::fill(DataReader &dr)
    {
    	throw Exception("Invalid field fill");
    }

    void RflField::fillArray(uint16_t size, DataReader &dr)
    {
    	throw Exception("Invalid field fillArray");
    }



    RflString::RflString(const std::string &defaultValue)
	: mValue(defaultValue)
	{
	}

    void RflString::fillArray(uint16_t size, DataReader &dr)
	{
		char *dataBuffer = new char[size*4+1]; // FIXME: RAII, maybe?
		dr.read(dataBuffer, size*4);
		dataBuffer[size] = '\0';

		mValue = std::string(dataBuffer);
	}

}
