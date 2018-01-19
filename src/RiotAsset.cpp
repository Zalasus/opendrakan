/*
 * RiotAsset.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include "RiotAsset.h"

#include "Exception.h"

namespace od
{

    RiotAsset::RiotAsset()
    {
    }

    RiotAsset::~RiotAsset()
    {
    }

    std::string RiotAsset::getName()
    {
        return "";
    }



    AssetFactory::AssetFactory(AssetType type, const char *dbExtension)
    : mType(type)
    , mDbExtension(dbExtension)
    {
        if(_getFactoryMap().find(type) != _getFactoryMap().end())
        {
            throw Exception("Double registration of asset factory");
        }

        _getFactoryMap()[type] = this;
    }

    AssetFactory::~AssetFactory()
    {
        // TODO: do we need to unregister here? on second thought, do we need this whole registration stuff?
    }

    AssetType AssetFactory::getType() const
    {
        return mType;
    }

    std::string AssetFactory::getDbExtension() const
    {
        return mDbExtension;
    }

    AssetFactory &AssetFactory::getFactoryForType(AssetType type)
    {
        if(_getFactoryMap().find(type) == _getFactoryMap().end())
        {
            throw Exception("No factory registered for given asset type");
        }

        return *(_getFactoryMap()[type]);
    }

    std::unordered_map<AssetType, AssetFactory*> &AssetFactory::_getFactoryMap()
    {
        static std::unordered_map<AssetType, AssetFactory*> sFactories;

        return sFactories;
    }

}



