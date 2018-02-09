/*
 * ObjectTemplate.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "ObjectTemplate.h"

#include "Logger.h"
#include "Database.h"

namespace od
{

    ObjectTemplate::ObjectTemplate(Database &db, RecordId classId)
    : Asset(db, classId)
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

        if(hasModel())
        {
        	mModel = getDatabase().getAssetAsModel(mModelRef);
        }

        Logger::debug() << "Loaded object template with name " << mTemplateName;
    }

}
