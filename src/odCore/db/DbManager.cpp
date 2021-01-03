/*
 * DbManager.cpp
 *
 *  Created on: 11 Jan 2018
 *      Author: zal
 */


#include <odCore/db/DbManager.h>

#include <odCore/Logger.h>
#include <odCore/Exception.h>

#include <odCore/db/Database.h>
#include <odCore/db/DependencyTable.h>

namespace odDb
{

    static constexpr size_t MAX_DEPENDENCY_DEPTH{100};


    DbManager::DbManager()
    {
    }

    DbManager::~DbManager()
    {
    }

    std::shared_ptr<Database> DbManager::loadDatabase(const od::FilePath &dbFilePath, size_t dependencyDepth)
    {
    	if(dependencyDepth > MAX_DEPENDENCY_DEPTH)
    	{
    		throw od::Exception("Dependency depth exceeded maximum. Possible undetected circular dependency?");
    	}

    	if(auto db = getDatabaseByPath(dbFilePath); db != nullptr)
        {
            return db;
        }

    	Logger::info() << "Loading database " << dbFilePath.str();

        auto newGlobalIndex = mNextGlobalIndex++;

        auto db = std::make_shared<Database>(dbFilePath, *this, newGlobalIndex);
        mLoadedDatabases[newGlobalIndex] = db;
        db->getDependencyTable()->setSelfRefDatabase(db);
        db->loadDbFileAndDependencies(dependencyDepth);

        return db;
    }

    std::shared_ptr<Database> DbManager::getDatabaseByPath(const od::FilePath &dbFilePath)
    {
    	for(auto &weakDb : mLoadedDatabases)
        {
            auto db = weakDb.second.lock();
        	if(db != nullptr && db->getDbFilePath() == dbFilePath)
        	{
        		return db;
        	}
        }

        return nullptr;
    }

    std::shared_ptr<Database> DbManager::getDatabaseByGlobalIndex(GlobalDatabaseIndex index)
    {
        return mLoadedDatabases[index].lock();
    }

    size_t DbManager::getLoadedDatabaseCount() const
    {
        size_t count = 0;

        for(auto &weakDb : mLoadedDatabases)
        {
        	if(!weakDb.second.expired())
        	{
        		++count;
        	}
        }

        return count;
    }

}
