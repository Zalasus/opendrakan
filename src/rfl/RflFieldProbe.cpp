/*
 * RflFieldProbe.cpp
 *
 *  Created on: 31 Jan 2018
 *      Author: zal
 */

#include "rfl/RflFieldProbe.h"

namespace od
{

    void RflFieldProbe::registerField(RflField &field, RflField::RflFieldType fieldType, const std::string &fieldName)
    {
        RflFieldReg reg =
        {
            .field = field,
            .fieldType = fieldType,
            .fieldName = fieldName
        };

        mFields.push_back(reg);
    }



}


