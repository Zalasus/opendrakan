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
#include <map>
#include <memory>
#include <vector>
#include <functional>

#include "RiotAsset.h"
#include "FilePath.h"
#include "SrscFile.h"
#include "TextureFactory.h"

namespace od
{

    class RiotDb;

	class DbManager;

    typedef std::reference_wrapper<RiotDb> RiotDbRef;

	/**
	 * Common interface for RiotDb and RiotLevel's database index lookup.
	 */
	class AssetRefTranslator
	{
	public:

	    virtual ~AssetRefTranslator() {}

	    virtual AssetPtr getAssetByRef(AssetType type, const AssetRef &ref) = 0;
	};

	class RiotDb : public AssetRefTranslator
	{
	public:

		RiotDb(FilePath dbFilePath, DbManager &dbManager);
		~RiotDb();

		FilePath getDbFilePath() const;

		void loadDbFileAndDependencies(size_t dependencyDepth);

		AssetPtr getAssetById(AssetType type, RecordId id);


		// implement AssetRefTranslator
        virtual AssetPtr getAssetByRef(AssetType type, const AssetRef &ref);


	private:


		FilePath mDbFilePath;
		DbManager &mDbManager;

		uint32_t mVersion;
		std::map<uint16_t, RiotDbRef> mDependencyMap;

		TextureFactory mTextureFactory;
	};
}

#endif /* INCLUDE_RIOTDB_H_ */
