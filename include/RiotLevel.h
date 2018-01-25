/*
 * RiotLevel.h
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */

#ifndef RIOTLEVEL_H_
#define RIOTLEVEL_H_

#include "FilePath.h"

#include <string>

#include "SimpleArray.h"
#include "RiotDb.h"
#include "DbManager.h"

namespace od
{

    class RiotLevel
    {
    public:

        RiotLevel(const FilePath &levelPath, DbManager &dbManager);


    private:

        struct DbRef
        {
            RiotDb *db;
            uint16_t index;
        };

        void _loadLevel();


        FilePath mLevelPath;
        DbManager &mDbManager;

        std::string mLevelName;
        uint32_t mMaxWidth;
        uint32_t mMaxHeight;
        SimpleArray<DbRef> mDatabases;
    };

}

#endif /* RIOTLEVEL_H_ */
