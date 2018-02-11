/*
 * Class.cpp
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#include "Class.h"

#include "Logger.h"
#include "Database.h"
#include "ClassFactory.h"

namespace od
{

    Class::Class(Database &db, RecordId classId)
    : Asset(db, classId)
    , mIconNumber(0)
    {
    }

    void Class::loadFromRecord(ClassFactory &factory, DataReader dr)
    {
        uint16_t rflClassType;

        dr >> mClassName
           >> DataReader::Ignore(2)
           >> mModelRef
           >> rflClassType;

        if(hasModel())
        {
        	mModel = getDatabase().getModelByRef(mModelRef);
        }
    }

}
