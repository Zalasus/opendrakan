/*
 * Field.cpp
 *
 *  Created on: 14 Oct 2018
 *      Author: zal
 */

#include <odCore/rfl/Field.h>

#include <odCore/Panic.h>

namespace odRfl
{

    void Field::fill(od::DataReader &dr)
    {
        OD_PANIC() << "Tried to fill() field without fill implementation (this probably means something tried to fill an array field from non-array data)";
    }

    void Field::fillArray(uint16_t size, od::DataReader &dr)
    {
        OD_PANIC() << "Tried to fillArray() field without fillArray implementation (this probably means something tried to fill a normal field from array data)";
    }


    String::String(const std::string &defaultValue)
    : mValue(defaultValue)
    {
    }

    void String::fillArray(uint16_t size, od::DataReader &dr)
    {
        std::vector<char> dataBuffer(size*4 + 1);
        dr.read(dataBuffer.data(), size*4);
        dataBuffer[size*4] = '\0';

        mValue.assign(dataBuffer.data());
    }

}
