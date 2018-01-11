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

		bool isDbLoaded(FilePath dbFilePath) const;

        /**
         * @param[in]   dbFile  Path to the *.db file defining the database to be loaded. Extension optional.
         */
		RiotDb &loadDb(FilePath dbFilePath);


	private:

		std::vector<RiotDb*> mRiotDbs;
	};

}

#endif /* INCLUDE_DBMANAGER_H_ */
