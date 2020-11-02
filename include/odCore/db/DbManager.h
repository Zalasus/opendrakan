/*
 * DbManager.h
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_DBMANAGER_H_
#define INCLUDE_DBMANAGER_H_

#include <memory>
#include <unordered_map>

#include <odCore/CTypes.h>
#include <odCore/FilePath.h>

namespace odDb
{

    class Database;

	class DbManager
	{
	public:

		DbManager();
		~DbManager();

        /**
         * @brief Loads a database if not already loaded and returns the Database object.
         *
         * @param[in]   dbFile              Path to the *.db file defining the database to be loaded
         * @param[in]   dependencyDepth     Depth of dependency loading. Used to prevent undectected circular dependencies. Only used by Database objects.
         */
		std::shared_ptr<Database> loadDatabase(const od::FilePath &dbFilePath, size_t dependencyDepth = 0);

		std::shared_ptr<Database> getDatabaseByPath(const od::FilePath &dbFilePath);
        std::shared_ptr<Database> getDatabaseByGlobalIndex(size_t index);

        /**
         * Note that this is racy. The DB manager only weakly owns databases, so the actual count
         * may change if another thread releases or loads a database.
         */
        size_t getLoadedDatabaseCount() const;

        template <typename F>
        void forEachLoadedDatabase(const F &f)
        {
            for(auto &weakDb : mLoadedDatabases)
            {
                if(auto db = weakDb.second.lock(); db != nullptr)
                {
                    f(db);
                }
            }
        }


	private:

        // FIXME: make sure a database that is unloaded, then loaded again gets the same global index!
        std::unordered_map<size_t, std::weak_ptr<Database>> mLoadedDatabases;
        size_t mNextGlobalIndex;
	};

}

#endif /* INCLUDE_DBMANAGER_H_ */
