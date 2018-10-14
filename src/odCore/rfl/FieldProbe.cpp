/*
 * FieldProbe.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: zal
 */


#include <odCore/rfl/FieldProbe.h>

#include <odCore/rfl/Field.h>

namespace odRfl
{

    void FieldProbe::beginCategory(const char *categoryName)
    {
    }

    void FieldProbe::registerField(Field &field, const char *fieldName)
    {
    }

    void FieldProbe::registerField(AssetRefField &field, const char *fieldName)
    {
        this->registerField(static_cast<Field&>(field), fieldName);
    }

}

