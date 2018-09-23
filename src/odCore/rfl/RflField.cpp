/*
 * RflField.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <odCore/rfl/RflField.h>

#include <odCore/Exception.h>

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
		std::vector<char> dataBuffer(size*4 + 1);
		dr.read(dataBuffer.data(), size*4);
		dataBuffer[size*4] = '\0';

		mValue.assign(dataBuffer.data());
	}

}
