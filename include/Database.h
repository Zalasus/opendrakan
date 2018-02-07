/*
 * RiotDb.h
 *
 *  Created on: 6 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_DATABASE_H_
#define INCLUDE_DATABASE_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <vector>
#include <functional>

#include "FilePath.h"
#include "SrscFile.h"
#include "Texture.h"
#include "TextureFactory.h"
#include "Asset.h"

namespace od
{

	class DbManager;
	class Database;


    typedef std::reference_wrapper<Database> DbRefWrapper;


    enum AssetType
    {
        ASSET_TEXTURE,
        ASSET_CLASS,
        ASSET_MODEL,
        ASSET_SOUND,
        ASSET_SEQUENCE
    };


	/**
	 * Common interface for classes with means to translate DB indices to
	 * actual databases, like Database and Level.
	 *
	 * TODO: perhaps move this into it's own header.
	 */
	class AssetProvider
	{
	public:

	    virtual ~AssetProvider() {}

	    virtual TexturePtr getAssetAsTexture(const AssetRef &ref) = 0;
	};


	class Database : public AssetProvider
	{
	public:

		Database(FilePath dbFilePath, DbManager &dbManager);
		~Database();

		FilePath getDbFilePath() const;

		void loadDbFileAndDependencies(size_t dependencyDepth);


		// implement AssetProvider
		virtual TexturePtr getAssetAsTexture(const AssetRef &ref);


	private:


		FilePath mDbFilePath;
		DbManager &mDbManager;

		uint32_t mVersion;
		std::map<uint16_t, DbRefWrapper> mDependencyMap;

		std::unique_ptr<TextureFactory> mTextureFactory;
	};
}

#endif /* INCLUDE_DATABASE_H_ */
