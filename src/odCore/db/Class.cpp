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

namespace od
{

    Class::Class(AssetProvider &ap, RecordId classId)
    : Asset(ap, classId)
    , mRflClassId(0)
    , mIconNumber(0)
    , mRfl(nullptr)
    , mRflClassRegistrar(nullptr)
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

        if(factory.getRfl() != nullptr)
        {
            mRfl = factory.getRfl();

            try
            {
                mRflClassRegistrar = factory.getRfl()->getRegistrarForClass(mRflClassId);

            }catch(NotFoundException &e)
            {
                Logger::debug() << "RflClass type " << std::hex << mRflClassId << std::dec <<
                    " of class '" << mClassName << "' not found. Probably unimplemented";
            }
        }
    }

    std::unique_ptr<RflClass> Class::makeInstance()
	{
        if(mRflClassRegistrar == nullptr)
        {
            Logger::debug() << "Tried to instantiate class without valid RFL class. Ignoring call as class is probably uinimplemented";
            return nullptr;
        }

    	Logger::debug() << "Instantiating class '" << mClassName << "' (" << std::hex << getAssetId() << std::dec << ")";

    	std::unique_ptr<RflClass> newInstance(mRflClassRegistrar->createInstance(mRfl));
        mClassBuilder.resetIndexCounter(); // in case of throw, do this BEFORE building so counter is always fresh TODO: pretty unelegant
        newInstance->probeFields(mClassBuilder);

        return newInstance;
	}

}
