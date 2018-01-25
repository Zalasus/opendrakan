/*
 * DbManager.h
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_DBMANAGER_H_
#define INCLUDE_DBMANAGER_H_

#include <string>

#include "FilePath.h"

namespace od
{

    class RiotDb;

	class DbManager
	{
	public:

		DbManager();
		~DbManager();

		bool isDbLoaded(const FilePath &dbFilePath) const;

        /**
         * @brief Loads a database if not already loaded and returns the the RiotDb object.
         *
         * @param[in]   dbFile              Path to the *.db file defining the database to be loaded. Extension is ignored.
         * @param[in]   dependencyDepth     Depth of dependency loading. Used to prevent undectected circular dependencies. Only used by RiotDb objects.
         */
		RiotDb &loadDb(const FilePath &dbFilePath, size_t dependencyDepth = 0);

		RiotDb &getDb(const FilePath &dbFilePath);


	private:

		std::vector<RiotDb*> mRiotDbs;
	};

}

#endif /* INCLUDE_DBMANAGER_H_ */
