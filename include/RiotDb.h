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
#include <unordered_map>
#include <memory>

#include "RiotAsset.h"
#include "SimpleArray.h"
#include "FilePath.h"
#include "SrscFile.h"

namespace od
{

    class RiotDb;

	struct Dependency
	{
	    uint16_t index;
	    RiotDb *db;
	};

	class DbManager;

	class RiotDb
	{
	public:

		RiotDb(FilePath dbFilePath, DbManager &dbManager);
		~RiotDb();

		FilePath getDbFilePath() const;

		SrscFile &getResourceContainer(AssetType type);

		void loadDbFileAndDependencies(size_t dependencyDepth);

		AssetPtr getAssetById(AssetType type, RecordId id);


	private:


		FilePath mDbFilePath;
		DbManager &mDbManager;

		uint32_t mVersion;
		SimpleArray<Dependency> mDependencies;

		std::unordered_map<AssetType, SrscFile*> mSrscMap;
	};

}

#endif /* INCLUDE_RIOTDB_H_ */
