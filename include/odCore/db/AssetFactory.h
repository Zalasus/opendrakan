/*
 * AssetFactory.h
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ASSETFACTORY_H_
#define INCLUDE_ASSETFACTORY_H_

#include <map>
#include <osg/ref_ptr>
#include <osg/Observer>

#include <odCore/FilePath.h>
#include <odCore/SrscFile.h>
#include <odCore/Logger.h>
#include <odCore/Exception.h>
#include <odCore/db/Asset.h>

namespace od
{
	class AssetProvider;

	/**
	 * Common interface for TextureFactory, AssetFactory etc.
	 */
	template <typename _AssetType>
	class AssetFactory : public osg::Observer
	{
	public:

	    typedef _AssetType AssetType;

		AssetFactory(AssetFactory &f) = delete;
		AssetFactory(const AssetFactory &f) = delete;
		virtual ~AssetFactory();

		inline AssetProvider &getAssetProvider() { return mAssetProvider; }
		inline SrscFile &getSrscFile() { return mSrscFile; }

		osg::ref_ptr<_AssetType> getAsset(RecordId assetId);


	protected:

		AssetFactory(AssetProvider &ap, SrscFile &assetContainer);

		/**
		 * @brief Interface method for asking child factories to load uncached asset.
		 *
		 * This method will get called by AssetFactory when an asset that is not yet cached is requested.
		 * The implementing class must load the asset with the given ID and return it, or return nullptr if
		 * it could not be found. In the latter case, AssetFactory will produce an appropriate error message and exception.
		 */
		virtual osg::ref_ptr<_AssetType> loadAsset(RecordId id) = 0;

		// override osg::Observer
		virtual void objectDeleted(void *object) override;


	private:

		_AssetType *_getAssetFromCache(RecordId id);
		void _addAssetToCache(RecordId id, _AssetType *asset);

		AssetProvider &mAssetProvider;
		SrscFile &mSrscFile;

		std::map<RecordId, _AssetType*> mAssetCache;
	};

	template <typename _AssetType>
	AssetFactory<_AssetType>::AssetFactory(AssetProvider &ap, SrscFile &assetContainer)
	: mAssetProvider(ap)
	, mSrscFile(assetContainer)
	{
	}

	template <typename _AssetType>
	AssetFactory<_AssetType>::~AssetFactory()
	{
	}

	template <typename _AssetType>
	osg::ref_ptr<_AssetType> AssetFactory<_AssetType>::getAsset(RecordId assetId)
	{
		_AssetType *cached = this->_getAssetFromCache(assetId);
	    if(cached != nullptr)
	    {
	        Logger::debug() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " found in cache";
	        return osg::ref_ptr<_AssetType>(cached);
	    }

	    // not cached. let implementation handle loading
	    Logger::debug() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " not found in cache. Loading from container " << mSrscFile.getFilePath().fileStr();
	    osg::ref_ptr<_AssetType> loaded = this->loadAsset(assetId);
	    if(loaded == nullptr)
	    {
	        Logger::error() << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " neither found in cache nor asset container " << mSrscFile.getFilePath().fileStr();
            throw NotFoundException("Asset not found in cache or asset container");
	    }
	    this->_addAssetToCache(assetId, loaded.get());

	    return loaded;
	}

	template <typename _AssetType>
	void AssetFactory<_AssetType>::objectDeleted(void *object)
	{
		_AssetType *asset = dynamic_cast<_AssetType*>(static_cast<osg::Referenced*>(object)); // a bit unsafe but osg gives us no choice
	    if(asset == nullptr)
	    {
	        Logger::warn() << AssetTraits<_AssetType>::name() << " factory was notified of deletion of non-" << AssetTraits<_AssetType>::name() << " object";
	        return;
	    }

	    RecordId assetId = asset->getAssetId();

	    Logger::debug() << "Unregistering " << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec << " from cache";

	    mAssetCache.erase(assetId);
	}

	template <typename _AssetType>
	_AssetType *AssetFactory<_AssetType>::_getAssetFromCache(RecordId assetId)
	{
		auto it = mAssetCache.find(assetId);
	    if(it != mAssetCache.end())
	    {
	        return it->second;
	    }

	    return nullptr;
	}

	template <typename _AssetType>
	void AssetFactory<_AssetType>::_addAssetToCache(RecordId assetId, _AssetType *asset)
	{
		if(_getAssetFromCache(assetId) != nullptr)
		{
			Logger::warn() << "Ignoring double caching of " << AssetTraits<_AssetType>::name() << " " << std::hex << assetId << std::dec;
			return;
		}

		asset->addObserver(this);

		mAssetCache[assetId] = asset;
	}

}

#endif /* INCLUDE_ASSETFACTORY_H_ */
