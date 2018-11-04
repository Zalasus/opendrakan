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
#include <odCore/db/Asset.h>

namespace odDb
{
	class AssetProvider;

	/**
	 * Common interface for TextureFactory, AssetFactory etc.
	 */
	template <typename _AssetType>
	class AssetFactory : public AssetReferenceObserver
	{
	public:

	    typedef _AssetType AssetType;

		AssetFactory(AssetFactory &f) = delete;
		AssetFactory(const AssetFactory &f) = delete;
		virtual ~AssetFactory();

		inline AssetProvider &getAssetProvider() { return mAssetProvider; }
		inline od::SrscFile &getSrscFile() { return mSrscFile; }

		AssetPtr<_AssetType> getAsset(od::RecordId assetId);


	protected:

		AssetFactory(AssetProvider &ap, od::SrscFile &assetContainer);

		/**
		 * @brief Interface method for asking child factories to load uncached asset.
		 *
		 * This method will get called by AssetFactory when an asset that is not yet cached is requested.
		 * The implementing class must load the asset with the given ID and return it, or return nullptr if
		 * it could not be found. In the latter case, AssetFactory will produce an appropriate error message and exception.
		 */
		virtual AssetPtr<_AssetType> loadAsset(od::RecordId id) = 0;

		// implement AssetReferenceObserver
		virtual void onLastReferenceDestroyed(Asset *asset) override;


	private:

		_AssetType *_getAssetFromCache(od::RecordId id);
		void _addAssetToCache(od::RecordId id, _AssetType *asset);

		AssetProvider &mAssetProvider;
		od::SrscFile &mSrscFile;

		std::map<od::RecordId, _AssetType*> mAssetCache;
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
	AssetPtr<_AssetType> AssetFactory<_AssetType>::getAsset(od::RecordId assetId)
	{
		_AssetType *cached = this->_getAssetFromCache(assetId);
	    if(cached != nullptr)
	    {
	        Logger::debug() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " found in cache";
	        return AssetPtr<_AssetType>(cached);
	    }

	    // not cached. let implementation handle loading
	    Logger::debug() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " not found in cache. Loading from container " << mSrscFile.getFilePath().fileStr();
	    AssetPtr<_AssetType> loaded = this->loadAsset(assetId);
	    if(loaded == nullptr)
	    {
	        Logger::error() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " neither found in cache nor asset container " << mSrscFile.getFilePath().fileStr();
            throw od::NotFoundException("Asset not found in cache or asset container");
	    }
	    this->_addAssetToCache(assetId, loaded.get());

	    return loaded;
	}

	template <typename _AssetType>
	void AssetFactory<_AssetType>::onLastReferenceDestroyed(Asset *asset)
	{
		_AssetType *childAsset = dynamic_cast<_AssetType*>(asset); // FIXME: kinda unneccessary dyanmic cast. only used for type-checking
	    if(childAsset == nullptr)
	    {
	        Logger::warn() << AssetTraits<_AssetType>::name() << " factory was notified of deletion of non-" << AssetTraits<_AssetType>::name() << " object";
	        return;
	    }

	    od::RecordId assetId = childAsset->getAssetId();

	    Logger::debug() << "Unregistering " << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " from cache";

	    mAssetCache.erase(assetId);
	}

	template <typename _AssetType>
	_AssetType *AssetFactory<_AssetType>::_getAssetFromCache(od::RecordId assetId)
	{
		auto it = mAssetCache.find(assetId);
	    if(it != mAssetCache.end())
	    {
	        return it->second;
	    }

	    return nullptr;
	}

	template <typename _AssetType>
	void AssetFactory<_AssetType>::_addAssetToCache(od::RecordId assetId, _AssetType *asset)
	{
		if(_getAssetFromCache(assetId) != nullptr)
		{
			Logger::warn() << "Ignoring double caching of " << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec;
			return;
		}

		asset->setReferenceObserver(this);

		mAssetCache[assetId] = asset;
	}

}

#endif /* INCLUDE_ASSETFACTORY_H_ */
