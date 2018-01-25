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
    	for(RiotDb *db : mRiotDbs)
    	{
    		delete db;
    	}
    }

    bool DbManager::isDbLoaded(const FilePath &dbFilePath) const
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

    	}catch(Exception &e)
    	{
    	    // not loaded -> load
    	}

    	Logger::info() << "Loading db: " << dbFilePath.str() << " depth: " << dependencyDepth;

        RiotDb *db = new RiotDb(actualFilePath, *this); // FIXME: use some more elegant RAII here
        if(db == NULL)
        {
        	throw Exception("Could not allocate db object");
        }
        mRiotDbs.push_back(db);

        db->loadDbFileAndDependencies(dependencyDepth);

        return *db;
    }

    RiotDb &DbManager::getDb(const FilePath &dbFilePath)
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


