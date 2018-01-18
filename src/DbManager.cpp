/*
 * DbManager.cpp
 *
 *  Created on: 11 Jan 2018
 *      Author: zal
 */

#include "DbManager.h"

#include <algorithm>

#include "RiotDb.h"
#include "StringUtils.h"

namespace od
{

    DbManager::DbManager()
    {
    }

    bool DbManager::isDbLoaded(FilePath dbFilePath) const
    {
        auto pred = [&dbFilePath](RiotDb *db){ return db->getDbFilePath() == dbFilePath; };

        return std::find_if(mRiotDbs.begin(), mRiotDbs.end(), pred) != mRiotDbs.end();
    }

    RiotDb &DbManager::loadDb(FilePath dbFilePath)
    {
        std::string fullPath = dbFilePath.str();
        if(!StringUtils::endsWith(fullPath, ".db"))
        {
            fullPath += ".db";
        }

        RiotDb *db = new RiotDb(FilePath(fullPath), *this);
        mRiotDbs.push_back(db);

        return *db;
    }

}


