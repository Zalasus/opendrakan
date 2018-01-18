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

#include "RiotAsset.h"
#include "SimpleArray.h"
#include "FilePath.h"
#include "SrscFile.h"

namespace od
{

	enum AssetType
	{
		ASSET_TEXTURE,
		ASSET_CLASS,
		ASSET_MODEL,
		ASSET_SOUND,
		ASSET_SEQUENCE
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


		static std::string getExtensionForAssetType(AssetType type);


	private:


		FilePath mDbFilePath;
		DbManager &mDbManager;

		uint32_t mVersion;
		SimpleArray<RiotDb*> mDependencies;

		std::unordered_map<AssetType, SrscFile*> mSrscMap;
	};

}

#endif /* INCLUDE_RIOTDB_H_ */
