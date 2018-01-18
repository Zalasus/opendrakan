/*
 * DbManager.cpp
 *
 *  Created on: 11 Jan 2018
 *      Author: zal
 */

#include "DbManager.h"

#include <iostream>

#include "RiotDb.h"
#include "StringUtils.h"
#include "Exception.h"

#define OD_MAX_DEPENDENCY_DEPTH 100

namespace od
{

    DbManager::DbManager()
    {
    }

    DbManager::~DbManager()
    {
    	for(RiotDb *db : mRiotDbs)
    	{
    		delete db;
    	}
    }

    bool DbManager::isDbLoaded(FilePath dbFilePath) const
    {
        for(RiotDb *db : mRiotDbs)
        {
        	if(db->getDbFilePath() == dbFilePath)
        	{
        		return true;
        	}
        }

        return false;
    }

    RiotDb &DbManager::loadDb(FilePath dbFilePath, size_t dependencyDepth)
    {
    	if(dependencyDepth > OD_MAX_DEPENDENCY_DEPTH)
    	{
    		throw Exception("Dependency depth exceeded maximum. Possible undetected circular dependency?");
    	}

    	if(isDbLoaded(dbFilePath))
    	{
    		throw Exception("Tried to load db that was already loaded.");
    	}

    	std::cout << "Loading db: " << dbFilePath.str() << " depth: " << dependencyDepth << std::endl;

        RiotDb *db = new RiotDb(dbFilePath, *this); // FIXME: use some more elegant RAII here
        if(db == NULL)
        {
        	throw Exception("Could not allocate db object");
        }
        mRiotDbs.push_back(db);

        db->loadDbFileAndDependencies(dependencyDepth);

        return *db;
    }

    RiotDb &DbManager::getDb(FilePath dbFilePath)
    {
    	for(RiotDb *db : mRiotDbs)
        {
        	if(db->getDbFilePath() == dbFilePath)
        	{
        		return *db;
        	}
        }

        throw Exception("Db with given path not loaded");
    }

}


