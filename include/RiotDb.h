/*
 * RiotDb.h
 *
 *  Created on: 6 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RIOTDB_H_
#define INCLUDE_RIOTDB_H_

#include <string>
#include <vector>

#include "RiotAsset.h"
#include "SimpleArray.h"
#include "FilePath.h"

namespace od
{

	class DbManager;

	class RiotDb
	{
	public:

		RiotDb(FilePath dbFilePath, DbManager &dbManager);

		FilePath getDbFilePath() const;


	private:

		void _loadDbFileAndDependencies();

		FilePath mDbFilePath;
		DbManager &mDbManager;

		uint32_t mVersion;
		SimpleArray<RiotDb*> mDependencies;
	};

}

#endif /* INCLUDE_RIOTDB_H_ */
