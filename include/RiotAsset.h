/*
 * RiotAsset.h
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RIOTASSET_H_
#define INCLUDE_RIOTASSET_H_

#include <unordered_map>
#include <memory>

#include "SrscFile.h"
#include "Exception.h"

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

	class RiotAsset
	{
	public:

		RiotAsset();
		virtual ~RiotAsset();

		virtual void loadFromRecord(SrscFile &srscFile, RecordId id) = 0;

		virtual std::string getName();

	};

	typedef std::shared_ptr<RiotAsset> AssetPtr;

	class AssetFactory
	{
	public:

	    AssetFactory(AssetType type, const char *dbExtension);
	    virtual ~AssetFactory();

	    AssetType getType() const;
	    std::string getDbExtension() const;

	    virtual AssetPtr createNewAsset() = 0;


	    static AssetFactory &getFactoryForType(AssetType type);


	private:

	    static std::unordered_map<AssetType, AssetFactory*> &_getFactoryMap();

	    AssetType mType;
	    std::string mDbExtension;
	};

	template <typename T>
	class AssetFactoryImpl : public AssetFactory
	{
	public:

	    AssetFactoryImpl(AssetType type, const char *dbExtension);

	    virtual AssetPtr createNewAsset();

	};

	template <typename T>
	AssetFactoryImpl<T>::AssetFactoryImpl(AssetType type, const char *dbExtension)
	: AssetFactory(type, dbExtension)
	{
	}

	template <typename T>
	AssetPtr AssetFactoryImpl<T>::createNewAsset()
	{
	    RiotAsset *newAsset = new T;
	    if(newAsset == nullptr)
	    {
	        throw Exception("Could not allocate new asset");
	    }

	    return AssetPtr(newAsset);
	}

}

#define OD_REGISTER_ASSET_TYPE(assetType, assetClass, dbExtension) static AssetFactoryImpl<assetClass> sAssetFectory_ ## assetClass(assetType, dbExtension);

#endif /* INCLUDE_RIOTASSET_H_ */
