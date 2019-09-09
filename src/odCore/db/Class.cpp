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

    Class::Class(AssetProvider &ap, od::RecordId classId, ClassFactory &factory)
    : Asset(ap, classId)
    , mClassFactory(factory)
    , mRflClassId(0)
    , mIconNumber(0)
    , mRfl(mClassFactory.getRfl())
    , mRflClassRegistrar(nullptr)
    {
    }

    void Class::load(od::SrscFile::RecordInputCursor cursor)
    {
        od::DataReader dr = cursor.getReader();

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

        if(mRfl != nullptr)
        {
            try
            {
                mRflClassRegistrar = mRfl->getRegistrarForClassId(mRflClassId);

            }catch(od::NotFoundException &e)
            {
                Logger::debug() << "RflClass type " << std::hex << mRflClassId << std::dec <<
                    " of class '" << mClassName << "' not found. Probably unimplemented";
            }
        }
    }

    std::unique_ptr<odRfl::ClassBase> Class::makeInstance()
	{
        if(mRflClassRegistrar == nullptr)
        {
            Logger::debug() << "Tried to instantiate class without valid RFL class. Ignoring call as class is probably uinimplemented";
            return nullptr;
        }

    	Logger::debug() << "Instantiating class '" << mClassName << "' (" << std::hex << getAssetId() << std::dec << ")";

    	std::unique_ptr<odRfl::ClassBase> newInstance(mRflClassRegistrar->makeInstance());
        mClassBuilder.resetIndexCounter(); // in case of throw, do this BEFORE building so counter is always fresh TODO: pretty unelegant
        newInstance->probeFields(mClassBuilder);
        newInstance->setRfl(*mRfl);

        return newInstance;
	}

    std::unique_ptr<odRfl::LevelObjectClassBase> Class::makeInstanceForLevelObject(od::LevelObject &obj)
    {
        std::unique_ptr<odRfl::LevelObjectClassBase> instance;

        if(mRflClassRegistrar == nullptr)
        {
            instance = std::make_unique<odRfl::DefaultObjectClass>();

        }else
        {
            std::unique_ptr<odRfl::ClassBase> newInstance = makeInstance();
            if(newInstance->getBaseType() != odRfl::ClassBaseType::LEVEL_OBJECT)
            {
                throw od::Exception("Tried to make level object with non-level-object class");
            }

            instance.reset(newInstance.release()->asLevelObjectBase());
        }

        instance->setLevelObject(obj);
        return instance;
    }

}
