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
#include <odCore/rfl/RflManager.h>

namespace odDb
{

    Class::Class(AssetProvider &ap, od::RecordId classId, ClassFactory &factory)
    : Asset(ap, classId)
    , mClassFactory(factory)
    , mRflClassId(0)
    , mIconNumber(0)
    , mTriedToCacheRegistrar(false)
    , mCachedRflClassRegistrar(nullptr)
    , mRfl(nullptr)
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
    }

    std::unique_ptr<odRfl::ClassBase> Class::makeInstance(odRfl::RflManager &rflManager)
	{
        odRfl::ClassRegistrar *registrar = _getRegistrar(rflManager);
        if(registrar == nullptr)
        {
            Logger::debug() << "Tried to instantiate class without valid RFL class. Ignoring call as class is probably uinimplemented";
            return nullptr;
        }

    	Logger::debug() << "Instantiating class '" << mClassName << "' (" << std::hex << getAssetId() << std::dec << ")";

    	std::unique_ptr<odRfl::ClassBase> newInstance(registrar->makeInstance());
        mClassBuilder.resetIndexCounter(); // in case of throw, do this BEFORE building so counter is always fresh TODO: pretty unelegant
        newInstance->probeFields(mClassBuilder);
        newInstance->setRfl(*mRfl);

        return newInstance;
	}

    std::unique_ptr<odRfl::LevelObjectClassBase> Class::makeInstanceForLevelObject(odRfl::RflManager &rflManager, od::LevelObject &obj)
    {
        std::unique_ptr<odRfl::LevelObjectClassBase> instance;

        if(_getRegistrar(rflManager) == nullptr)
        {
            instance = std::make_unique<odRfl::DefaultObjectClass>();

        }else
        {
            std::unique_ptr<odRfl::ClassBase> newInstance = makeInstance(rflManager);
            if(newInstance->getBaseType() != odRfl::ClassBaseType::LEVEL_OBJECT)
            {
                throw od::Exception("Tried to make level object with non-level-object class");
            }

            instance.reset(newInstance.release()->asLevelObjectBase());
        }

        instance->setLevelObject(obj);
        return instance;
    }

    odRfl::ClassRegistrar *Class::_getRegistrar(odRfl::RflManager &rflManager)
    {
        if(!mTriedToCacheRegistrar)
        {
            odRfl::Rfl *rfl = rflManager.getRfl(mClassFactory.getRflPath().fileStrNoExt());
            if(rfl == nullptr)
            {
                Logger::warn() << "RFL '" << mClassFactory.getRflPath() << "' needed for Instantiating class '" << mClassName << "' not loaded";

            } else
            {
                mRfl = rfl;
                try
                {
                    mCachedRflClassRegistrar = mRfl->getRegistrarForClassId(mRflClassId);

                }catch(od::NotFoundException &e)
                {
                    Logger::debug() << "RFL class type " << std::hex << mRflClassId << std::dec <<
                        " of class '" << mClassName << "' not found. Probably unimplemented";
                }
            }

            mTriedToCacheRegistrar = true;
        }

        return mCachedRflClassRegistrar;
    }

}
