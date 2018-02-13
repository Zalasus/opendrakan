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
#include "rfl/RiotFunctionLibrary.h"
#include "rfl/RflFieldProbe.h"

namespace od
{

    Class::Class(Database &db, RecordId classId)
    : Asset(db, classId)
    , mIconNumber(0)
    {
    }

    void Class::loadFromRecord(ClassFactory &factory, DataReader dr)
    {
        uint16_t rflClassId;

        dr >> mClassName
           >> DataReader::Ignore(2)
           >> mModelRef
           >> rflClassId
		   >> mIconNumber;

        try
        {
        	RflClassRegistrar &cr = RiotFunctionLibrary::getSingleton().getClassRegistrarById(rflClassId);

        	RflClassBuilder builder;
        	builder.readFieldRecord(dr, false);
        	mRflClass = cr.createClassInstance(builder);

        	builder.fillFields();

        }catch(NotFoundException &e)
        {
        	// ignore these for now as there are way more RflClasses than we have implemented
        	Logger::warn() << "Loaded class with unknown RFLClass " << std::hex << rflClassId << std::dec;
        }

        if(hasModel())
        {
        	mModel = getDatabase().getModelByRef(mModelRef);
        }
    }

}
