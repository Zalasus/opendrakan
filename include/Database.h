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
#include "Asset.h"
#include "AssetProvider.h"
#include "TextureFactory.h"
#include "ModelFactory.h"
#include "ClassFactory.h"

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

	class Database : public AssetProvider
	{
	public:

		Database(FilePath dbFilePath, DbManager &dbManager);
		~Database();

		FilePath getDbFilePath() const;

		void loadDbFileAndDependencies(size_t dependencyDepth);


		// implement AssetProvider
		virtual TexturePtr getTextureByRef(const AssetRef &ref);
	    virtual ClassPtr   getClassByRef(const AssetRef &ref);
	    virtual ModelPtr   getModelByRef(const AssetRef &ref);

	    // override AssetProvider
	    virtual TexturePtr getTexture(RecordId recordId);
	    virtual ClassPtr   getClass(RecordId recordId);
	    virtual ModelPtr   getModel(RecordId recordId);


	private:


		FilePath mDbFilePath;
		DbManager &mDbManager;

		uint32_t mVersion;
		std::map<uint16_t, DbRefWrapper> mDependencyMap;

		std::unique_ptr<TextureFactory> mTextureFactory;
		std::unique_ptr<ModelFactory> mModelFactory;
		std::unique_ptr<ClassFactory> mClassFactory;
	};
}

#endif /* INCLUDE_DATABASE_H_ */
