/*
 * AssetFactory.h
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ASSETFACTORY_H_
#define INCLUDE_ASSETFACTORY_H_

#include <map>

#include <odCore/FilePath.h>
#include <odCore/SrscFile.h>
#include <odCore/Logger.h>
#include <odCore/Exception.h>
#include <odCore/WeakRefPtr.h>

#include <odCore/db/Asset.h>

namespace odDb
{
	class AssetProvider;

	/**
	 * Common interface for TextureFactory, AssetFactory etc.
	 */
	template <typename _AssetType>
	class AssetFactory
	{
	public:

	    typedef _AssetType AssetType;

		AssetFactory(AssetFactory &f) = delete;
		AssetFactory(const AssetFactory &f) = delete;
		virtual ~AssetFactory();

		inline AssetProvider &getAssetProvider() { return mAssetProvider; }
		inline od::SrscFile &getSrscFile() { return mSrscFile; }

		od::RefPtr<_AssetType> getAsset(od::RecordId assetId);


	protected:

		AssetFactory(AssetProvider &ap, od::SrscFile &assetContainer);

		/**
		 * @brief Interface method for asking child factories to load uncached asset.
		 *
		 * This method will get called by AssetFactory when an asset that is not yet cached is requested.
		 * The implementing class must load the asset with the given ID and return it, or return nullptr if
		 * it could not be found. In the latter case, AssetFactory will produce an appropriate error message and exception.
		 */
		virtual od::RefPtr<_AssetType> loadAsset(od::RecordId id) = 0;


	private:

		AssetProvider &mAssetProvider;
		od::SrscFile &mSrscFile;

		std::map<od::RecordId, od::WeakRefPtr<_AssetType>> mAssetCache;
	};


	template <typename _AssetType>
	AssetFactory<_AssetType>::AssetFactory(AssetProvider &ap, od::SrscFile &assetContainer)
	: mAssetProvider(ap)
	, mSrscFile(assetContainer)
	{
	}

	template <typename _AssetType>
	AssetFactory<_AssetType>::~AssetFactory()
	{
	}

	template <typename _AssetType>
	od::RefPtr<_AssetType> AssetFactory<_AssetType>::getAsset(od::RecordId assetId)
	{
	    auto it = mAssetCache.find(assetId);
        if(it != mAssetCache.end() && it->second != nullptr)
        {
            Logger::debug() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " found in cache";
            return od::RefPtr<_AssetType>(it->second.get());
        }

        // asset was not cached or got deleted. let implementation handle loading
	    Logger::debug() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " not found in cache. Loading from container " << mSrscFile.getFilePath().fileStr();
	    od::RefPtr<_AssetType> loaded = this->loadAsset(assetId);
	    if(loaded == nullptr)
	    {
	        Logger::error() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " neither found in cache nor asset container " << mSrscFile.getFilePath().fileStr();
            throw od::NotFoundException("Asset not found in cache or asset container");
	    }

	    if(it != mAssetCache.end())
	    {
	        it->second = loaded;

	    }else
	    {
	        mAssetCache[assetId] = od::WeakRefPtr<_AssetType>(loaded);
	    }

	    return loaded;
	}

}

#endif /* INCLUDE_ASSETFACTORY_H_ */
