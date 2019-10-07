/*
 * DbManager.cpp
 *
 *  Created on: 11 Jan 2018
 *      Author: zal
 */


#include <odCore/db/DbManager.h>

#include <odCore/Logger.h>
#include <odCore/StringUtils.h>
#include <odCore/Exception.h>
#include <odCore/db/Database.h>

#define OD_MAX_DEPENDENCY_DEPTH 100

namespace odDb
{

    DbManager::DbManager()
    {
    }

    DbManager::~DbManager()
    {
    }

    bool DbManager::isDbLoaded(const od::FilePath &dbFilePath) const
    {
        for(std::shared_ptr<Database> db : mRiotDbs)
        {
        	if(db->getDbFilePath() == dbFilePath)
        	{
        		return true;
        	}
        }

        return false;
    }

    Database &DbManager::loadDb(const od::FilePath &dbFilePath, size_t dependencyDepth)
    {
    	if(dependencyDepth > OD_MAX_DEPENDENCY_DEPTH)
    	{
    		throw od::Exception("Dependency depth exceeded maximum. Possible undetected circular dependency?");
    	}

    	// force the right extension
    	od::FilePath actualFilePath = dbFilePath.ext(".db");

    	try
    	{
    	    Database &db = getDb(actualFilePath);
    	    return db;

    	}catch(od::NotFoundException &e)
    	{
    	    // not loaded -> load
    	}

    	Logger::info() << "Loading database " << dbFilePath.str();

        auto db = std::make_shared<Database>(actualFilePath, *this);

        mRiotDbs.push_back(db);

        db->loadDbFileAndDependencies(dependencyDepth);

        return *db;
    }

    Database &DbManager::getDb(const od::FilePath &dbFilePath)
    {
    	for(std::shared_ptr<Database> db : mRiotDbs)
        {
        	if(db->getDbFilePath() == dbFilePath)
        	{
        		return *db;
        	}
        }

        throw od::NotFoundException("Database with given path not loaded");
    }

}


