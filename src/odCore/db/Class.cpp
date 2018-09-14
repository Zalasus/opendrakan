/*
 * Class.cpp
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#include <odCore/db/Class.h>

#include <odCore/Logger.h>
#include <odCore/Exception.h>
#include <odCore/db/ClassFactory.h>
#include <odCore/db/AssetProvider.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/RflFieldProbe.h>

namespace od
{

    Class::Class(AssetProvider &ap, RecordId classId)
    : Asset(ap, classId)
    , mRflClassId(0)
    , mIconNumber(0)
    {
    }

    void Class::loadFromRecord(ClassFactory &factory, DataReader dr)
    {
        dr >> mClassName
           >> DataReader::Ignore(2)
           >> mModelRef
           >> mRflClassId
		   >> mIconNumber;

        mClassBuilder.readFieldRecord(dr, false);

        if(mModelRef.assetId != 0)
        {
            try
            {
                mModel = this->getAssetProvider().getModelByRef(mModelRef);

            }catch(NotFoundException &e)
            {
                Logger::warn() << "Model of class " << mClassName << " not found. Leaving invisible";
                mModel = nullptr;
            }
        }
    }

    std::unique_ptr<odRfl::RflClass> Class::makeInstance()
	{
    	Logger::debug() << "Instantiating class '" << mClassName << "' (" << std::hex << getAssetId() << std::dec << ")";

    	try
        {
        	odRfl::RflClassRegistrar &cr = odRfl::Rfl::getSingleton().getClassRegistrarById(mRflClassId);

        	std::unique_ptr<odRfl::RflClass> newInstance = cr.createClassInstance();// FIXME: make sure this does not throw NotFoundException or cause unwanted catches
        	mClassBuilder.resetIndexCounter(); // in case of throw, do this BEFORE building so counter is always fresh TODO: pretty unelegant
        	newInstance->probeFields(mClassBuilder);

        	return newInstance;


        }catch(NotFoundException &e)
        {
        	// ignore these for now as there are way more RflClasses than we have implemented
        	//Logger::warn() << "Tried to instantiate class with unknown RFLClass " << std::hex << mRflClassId << std::dec;
        	Logger::debug() << "RflClass type " << std::hex << mRflClassId << std::dec <<
        			" of class '" << mClassName << "' not found. Probably unimplemented";
        }

        return nullptr;
	}

}
