/*
 * RiotLevel.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */

#include "RiotLevel.h"

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

		dr.ignore(4*(dummyLength + 1));
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
        }
    }

    void RiotLevel::_loadLayers(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordTypeId(0x0001, 0));

    	uint32_t layerCount;
    	dr >> layerCount;

    	Logger::info() << "Level has " << layerCount << " layers";
    	mLayers.resize(layerCount);

    	dr.ignore(4);

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		Logger::info() << "Loading layer " << i;

    		Layer layer;
    		layer.load(dr);

    		mLayers[i] = layer;
    	}
    }

}


