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
#include <odCore/rfl/DefaultObjectClass.h>

namespace odDb
{

    Class::Class(AssetProvider &ap, od::RecordId classId)
    : Asset(ap, classId)
    , mRflClassId(0)
    , mIconNumber(0)
    , mRfl(nullptr)
    , mRflClassRegistrar(nullptr)
    {
    }

    void Class::loadFromRecord(ClassFactory &factory, od::DataReader dr)
    {
        dr >> mClassName
           >> od::DataReader::Ignore(2)
           >> mModelRef
           >> mRflClassId
		   >> mIconNumber;

        mClassBuilder.readFieldRecord(dr);

        if(mModelRef.assetId != 0)
        {
            try
            {
                mModel = this->getAssetProvider().getAssetByRef<Model>(mModelRef);

            }catch(od::NotFoundException &e)
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

            }catch(od::NotFoundException &e)
            {
                Logger::debug() << "RflClass type " << std::hex << mRflClassId << std::dec <<
                    " of class '" << mClassName << "' not found. Probably unimplemented";
            }
        }
    }

    std::unique_ptr<odRfl::RflClass> Class::makeInstance()
	{
        if(mRflClassRegistrar == nullptr)
        {
            Logger::debug() << "Tried to instantiate class without valid RFL class. Ignoring call as class is probably uinimplemented";
            return nullptr;
        }

    	Logger::debug() << "Instantiating class '" << mClassName << "' (" << std::hex << getAssetId() << std::dec << ")";

    	std::unique_ptr<odRfl::RflClass> newInstance(mRflClassRegistrar->createInstance(mRfl));
        mClassBuilder.resetIndexCounter(); // in case of throw, do this BEFORE building so counter is always fresh TODO: pretty unelegant
        newInstance->probeFields(mClassBuilder);

        return newInstance;
	}

    std::unique_ptr<odRfl::RflClass> Class::makeInstanceForLevelObject(od::LevelObject &obj)
    {
        if(mRflClassRegistrar == nullptr)
        {
            return std::make_unique<odRfl::DefaultObjectClass>();
        }

        Logger::debug() << "Instantiating class '" << mClassName << "' (" << std::hex << getAssetId() << std::dec << ")";

        std::unique_ptr<odRfl::RflClass> newInstance(mRflClassRegistrar->createInstance(mRfl));
        mClassBuilder.resetIndexCounter(); // in case of throw, do this BEFORE building so counter is always fresh TODO: pretty unelegant
        newInstance->probeFields(mClassBuilder);

        return newInstance;
    }

}
