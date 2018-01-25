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
        SrscFile file(mLevelPath);

        DataReader dr(file.getStreamForRecordTypeId(0x0000, 0));
        dr  >> mLevelName
            >> mMaxWidth
            >> mMaxHeight;

        uint32_t dbRefCount;
        dr >> dbRefCount;

        Logger::info() << "Lvl has " << dbRefCount << " dependencies";

        mDatabases.allocate(dbRefCount);

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

            Logger::info() << "Lvl has dependency: " << dbPath.str();
        }
    }

}


