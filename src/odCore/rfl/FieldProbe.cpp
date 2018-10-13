/*
 * FieldProbe.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: zal
 */


#include <odCore/rfl/FieldProbe.h>

namespace odRfl
{

    void FieldProbe::beginCategory(const char *categoryName)
    {
    }

    void FieldProbe::registerField(RflField &field, const char *fieldName)
    {
    }

    void FieldProbe::registerField(RflAssetRef &field, const char *fieldName)
    {
        this->registerField((RflField&)field, fieldName);
    }

}

