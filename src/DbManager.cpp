/*
 * DbManager.cpp
 *
 *  Created on: 11 Jan 2018
 *      Author: zal
 */

#include "DbManager.h"

#include "Logger.h"
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
    }

    bool DbManager::isDbLoaded(const FilePath &dbFilePath) const
    {
        for(std::shared_ptr<RiotDb> db : mRiotDbs)
        {
        	if(db->getDbFilePath() == dbFilePath)
        	{
        		return true;
        	}
        }

        return false;
    }

    RiotDb &DbManager::loadDb(const FilePath &dbFilePath, size_t dependencyDepth)
    {
    	if(dependencyDepth > OD_MAX_DEPENDENCY_DEPTH)
    	{
    		throw Exception("Dependency depth exceeded maximum. Possible undetected circular dependency?");
    	}

    	// force the right extension
    	FilePath actualFilePath = dbFilePath.ext(".db");

    	try
    	{
    	    RiotDb &db = getDb(actualFilePath);
    	    return db;

    	}catch(NotFoundException &e)
    	{
    	    // not loaded -> load
    	}

    	Logger::info() << "Loading db: " << dbFilePath.str() << (dependencyDepth ? " due to dependency" : "");

        std::shared_ptr<RiotDb> db(new RiotDb(actualFilePath, *this));

        mRiotDbs.push_back(db);

        db->loadDbFileAndDependencies(dependencyDepth);

        return *db;
    }

    RiotDb &DbManager::getDb(const FilePath &dbFilePath)
    {
    	for(std::shared_ptr<RiotDb> db : mRiotDbs)
        {
        	if(db->getDbFilePath() == dbFilePath)
        	{
        		return *db;
        	}
        }

        throw NotFoundException("Database with given path not loaded");
    }

}


