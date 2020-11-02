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

    	// force the right extension
    	od::FilePath actualFilePath = dbFilePath.ext(".db");

    	if(auto db = getDatabaseByPath(actualFilePath); db != nullptr)
        {
            return db;
        }

    	Logger::info() << "Loading database " << dbFilePath.str();

        auto newGlobalIndex = mNextGlobalIndex++;

        auto db = std::make_shared<Database>(actualFilePath, *this, newGlobalIndex);
        mLoadedDatabases[newGlobalIndex] = db;
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

    std::shared_ptr<Database> DbManager::getDatabaseByGlobalIndex(size_t index)
    {
        return mLoadedDatabases[index].lock();
    }

}
