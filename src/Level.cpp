/*
 * RiotLevel.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */

#include "Level.h"

#include <algorithm>

#include "OdDefines.h"
#include "SrscRecordTypes.h"
#include "SrscFile.h"
#include "Logger.h"
#include "ZStream.h"
#include "Exception.h"

namespace od
{

    Level::Level(const FilePath &levelPath, DbManager &dbManager)
    : mLevelPath(levelPath)
    , mDbManager(dbManager)
    , mMaxWidth(0)
    , mMaxHeight(0)
    {
        _loadLevel();
    }

    const char *Level::libraryName() const
    {
    	return OD_LIB_NAME;
    }

    const char *Level::className() const
    {
    	return "RiotLevel";
    }

    void Level::_loadLevel()
    {
    	Logger::info() << "Loading level " << mLevelPath.str();

        SrscFile file(mLevelPath);

        _loadNameAndDeps(file);
        _loadLayers(file);
        //_loadLayerGroups(file); unnecessary, as this is probably just an editor thing

        Logger::info() << "Level loaded successfully";
    }

    void Level::_loadNameAndDeps(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordType(OD_SRSC_LEVEL_NAME));

    	dr  >> mLevelName
            >> mMaxWidth
            >> mMaxHeight;

        uint32_t dbRefCount;
        dr >> dbRefCount;

        Logger::verbose() << "Level depends on " << dbRefCount << " databases";

        for(size_t i = 0; i < dbRefCount; ++i)
        {
        	uint16_t dbIndex;
            dr  >> dbIndex;
            dr.ignore(2);

            std::string dbPathStr;
            dr >> dbPathStr;

            FilePath dbPath(dbPathStr, mLevelPath.dir());
            Database &db = mDbManager.loadDb(dbPath.adjustCase());

            Logger::debug() << "Level dependency index " << dbIndex << ": " << dbPath;

            mDependencyMap.insert(std::pair<uint16_t, DbRefWrapper>(dbIndex, db));
        }
    }

    void Level::_loadLayers(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordType(OD_SRSC_LEVEL_LAYERS));

    	uint32_t layerCount;
    	dr >> layerCount;

    	mLayers.reserve(layerCount);

    	Logger::verbose() << "Level has " << layerCount << " layers";

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		LayerPtr layer(new Layer(*this));
    		layer->loadDefinition(dr);

    		mLayers.push_back(layer);
    	}

    	dr.ignore(4);

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		uint32_t zlibStuffSize;
			dr >> zlibStuffSize;

			size_t zlibOffset = dr.getStream().tellg();

			ZStream zstr(dr.getStream());
			DataReader zdr(zstr);
			mLayers[i]->loadPolyData(zdr);
			zstr.seekToEndOfZlib();

			if((size_t)dr.getStream().tellg() != zlibOffset + zlibStuffSize)
			{
				throw IoException("ZStream read either too many or too few bytes");
			}

			mLayers[i]->buildGeometry();

			this->addChild(mLayers[i].get());
    	}
    }

    void Level::_loadLayerGroups(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordType(OD_SRSC_LEVEL_LAYERGROUPS));

    	uint32_t groupCount;
    	dr >> groupCount;

    	for(size_t i = 0; i < groupCount; ++i)
    	{
    		std::string groupName;
    		uint32_t layerCount;

    		dr >> groupName
			   >> layerCount;

    		Logger::info() << "Group " << groupName << " has " << layerCount << " layers:";

    		for(size_t j = 0; j < layerCount; ++j)
    		{
    			uint32_t layer;
    			dr >> layer;
    		}

    	}
    }

    void Level::_loadObjects(SrscFile &file)
    {
    	SrscFile::DirIterator objectRecord = file.getDirIteratorByType(OD_SRSC_LEVEL_OBJECTS);
    	if(objectRecord == file.getDirectoryEnd())
    	{
    		return; // if record does not appear level has no objects
    	}

    	DataReader dr(file.getStreamForRecord(objectRecord));

    	uint16_t objectCount;
    	dr >> objectCount;

    	/*mObjects.reserve(objectCount);

    	for(size_t i = 0; i < objectCount; ++i)
    	{
    		uint32_t index;
    		dr >> index;

    		uint16_t classId;
    		uint16_t classDb;
    		dr >> classId
			   >> classDb;

    		dr.ignore(4);
    	}*/
    }

    TexturePtr Level::getAssetAsTexture(const AssetRef &ref)
	{
    	Logger::debug() << "Requested texture " << ref.assetId << " from level dependency " << ref.dbIndex;

        auto it = mDependencyMap.find(ref.dbIndex);
        if(it == mDependencyMap.end())
        {
        	Logger::error() << "Database index " << ref.dbIndex << " not found in level dependencies";

            throw NotFoundException("Database index not found in level dependencies");
        }

        // TODO: instead of creating a new AssetRef everytime, why not add a getAsset method that just takes an ID?
        AssetRef foreignRef = ref;
        foreignRef.dbIndex = 0;
        return it->second.get().getAssetAsTexture(foreignRef);
    }

    ModelPtr Level::getAssetAsModel(const AssetRef &ref)
    {
    	Logger::debug() << "Requested model " << ref.assetId << " from level dependency " << ref.dbIndex;

        auto it = mDependencyMap.find(ref.dbIndex);
        if(it == mDependencyMap.end())
        {
        	Logger::error() << "Database index " << ref.dbIndex << " not found in level dependencies";

            throw NotFoundException("Database index not found in level dependencies");
        }

        // TODO: instead of creating a new AssetRef everytime, why not add a getAsset method that just takes an ID?
        AssetRef foreignRef = ref;
        foreignRef.dbIndex = 0;
        return it->second.get().getAssetAsModel(foreignRef);
    }
}


