/*
 * ObjectTemplate.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "ObjectTemplate.h"

namespace od
{

    ObjectTemplate::ObjectTemplate(RecordId classId)
    : Asset(classId)
    , mIconNumber(0)
    {
    }

    void ObjectTemplate::loadFromRecord(ObjectTemplateFactory &factory, DataReader dr)
    {
        uint16_t rflClassType;

        dr >> mTemplateName
           >> DataReader::Ignore(2)
           >> mModelRef
           >> rflClassType;
    }

}
