/*
 * Class.cpp
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#include <odCore/db/Class.h>

#include <odCore/Engine.h>
#include <odCore/Logger.h>
#include <odCore/Exception.h>

#include <odCore/db/ClassFactory.h>
#include <odCore/db/DependencyTable.h>

#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/RflManager.h>

namespace odDb
{

    Class::Class(ClassFactory &factory)
    : mClassFactory(factory)
    , mRflClassId(0)
    , mIconNumber(0)
    , mCachedRflClassFactory(nullptr)
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

        mFieldLoader.loadFromRecord(dr, odRfl::FieldLoaderProbe::RecordFormat::CLASS);
    }

    void Class::fillFields(odRfl::FieldBundle &fieldBundle)
    {
        mFieldLoader.reset(); // in case of throw, do this BEFORE building so counter is always fresh
        fieldBundle.probeFields(mFieldLoader);
    }

    std::unique_ptr<odRfl::ClassBase> Class::makeInstance(od::Engine &engine)
	{
        odRfl::ClassFactory *factory = getRflClassFactory(engine.getRflManager());
        if(factory == nullptr)
        {
            Logger::debug() << "Tried to instantiate class without valid RFL class. Ignoring call as class is probably uinimplemented";
            return nullptr;
        }

    	Logger::debug() << "Instantiating class '" << mClassName << "' (" << std::hex << getAssetId() << std::dec << ")";

    	std::unique_ptr<odRfl::ClassBase> newInstance = factory->makeInstance(engine);
        if(newInstance != nullptr)
        {
            fillFields(newInstance->getFields());
        }

        return newInstance;
	}

    std::shared_ptr<odDb::Model> Class::getOrLoadModel()
    {
        if(!hasModel())
        {
            return nullptr;

        }else if(mCachedModel == nullptr)
        {
            mCachedModel = getDependencyTable()->loadAsset<odDb::Model>(mModelRef);
        }

        return mCachedModel;
    }

    odRfl::ClassFactory *Class::getRflClassFactory(odRfl::RflManager &rflManager)
    {
        if(mCachedRflClassFactory == nullptr)
        {
            odRfl::Rfl *rfl = rflManager.getRfl(mClassFactory.getRflPath().fileStrNoExt());
            if(rfl == nullptr)
            {
                Logger::warn() << "RFL '" << mClassFactory.getRflPath() << "' needed for instantiating class '" << mClassName << "' not loaded";

            } else
            {
                try
                {
                    mCachedRflClassFactory = rfl->getFactoryForClassId(mRflClassId);

                }catch(od::NotFoundException &e)
                {
                    Logger::debug() << "RFL class type " << std::hex << mRflClassId << std::dec <<
                        " of class '" << mClassName << "' not found. Probably unimplemented";
                }
            }
        }

        return mCachedRflClassFactory;
    }

}
