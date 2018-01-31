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

    RflField::RflField(RflFieldProbe *probe, RflFieldType type, const char *fieldName)
    {
        if(probe != nullptr)
        {
            probe->registerField(*this, type, fieldName);
        }
    }

}
