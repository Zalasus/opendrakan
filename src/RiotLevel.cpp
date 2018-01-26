/*
 * RiotLevel.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */

#include "RiotLevel.h"

#include <algorithm>

#include "SrscFile.h"
#include "Logger.h"

namespace od
{


	void Layer::load(DataReader &dr)
	{
		dr 	>> width
			>> height
			>> type
			>> origin_x
			>> origin_y
			>> world_height
			>> layer_name
			>> flags
			>> light_direction
			>> light_ascension
			>> light_color
			>> ambient_color
			>> light_dropoff_type
			>> dummyLength;

		dummyField.reserve(dummyLength+1);
		for(size_t i = 0; i < dummyLength + 1; ++i)
		{
			uint32_t v;
			dr >> v;

			dummyField.push_back(v);
		}
	}



    RiotLevel::RiotLevel(const FilePath &levelPath, DbManager &dbManager)
    : mLevelPath(levelPath)
    , mDbManager(dbManager)
    , mMaxWidth(0)
    , mMaxHeight(0)
    {
        _loadLevel();
    }

    void RiotLevel::_loadLevel()
    {
    	Logger::info() << "Loading level " << mLevelPath.str();

        SrscFile file(mLevelPath);

        _loadNameAndDeps(file);
        _loadLayers(file);
        _loadLayerGroups(file);
    }

    void RiotLevel::_loadNameAndDeps(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordTypeId(0x0000, 0));

    	dr  >> mLevelName
            >> mMaxWidth
            >> mMaxHeight;

        uint32_t dbRefCount;
        dr >> dbRefCount;

        Logger::info() << "Level has " << dbRefCount << " dependencies";

        mDatabases.reserve(dbRefCount);

        for(size_t i = 0; i < dbRefCount; ++i)
        {
            DbRef ref;

            dr  >> ref.index;
            dr.ignore(2);

            std::string dbPathStr;
            dr >> dbPathStr;

            FilePath dbPath(dbPathStr, mLevelPath.dir());
            RiotDb &db = mDbManager.loadDb(dbPath);

            ref.db = &db;

            mDatabases[i] = ref;

            Logger::info() << ref.index << ": " << ref.db->getDbFilePath().str();
        }
    }

    void RiotLevel::_loadLayers(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordTypeId(0x0001, 0));

    	uint32_t layerCount;
    	dr >> layerCount;

    	Logger::info() << "Level has " << layerCount << " layers";
    	mLayers.reserve(layerCount);

    	dr.ignore(4);

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		Logger::info() << "Loading layer " << i;

    		Layer layer;
    		layer.load(dr);

    		mLayers.push_back(layer);

    		Logger::info() << "    name = " << layer.layer_name;
    		Logger::info() << "    n = " << layer.getDummyField().size();
    		for(uint32_t v : layer.getDummyField())
    		{
    			Logger::info() << "        " << v;
    		}
    	}

    	for(size_t i = 0; i < layerCount; ++i)
    	{
			uint32_t zlibStuffSize;
			dr >> zlibStuffSize;

			uint16_t zlibHeader;
			dr >> zlibHeader;

			Logger::info() << "The zlib stuff has " << zlibStuffSize << " bytes, hdr: " << std::hex << zlibHeader << std::dec;
			dr.ignore(zlibStuffSize-2);
    	}
    }

    void RiotLevel::_loadLayerGroups(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordTypeId(0x0002, 0));

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

    			Logger::info() << "    " << std::hex << layer << std::dec;
    		}

    	}
    }
}


