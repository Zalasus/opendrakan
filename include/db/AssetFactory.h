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

#include "FilePath.h"
#include "SrscFile.h"
#include "Logger.h"

namespace od
{
	class Database;

	/**
	 * Common interface for TextureFactory, AssetFactory etc.
	 */
	template <typename _AssetType>
	class AssetFactory : public osg::Observer
	{
	public:

		AssetFactory(AssetFactory &f) = delete;
		AssetFactory(const AssetFactory &f) = delete;
		virtual ~AssetFactory();

		inline Database &getDatabase() { return mDatabase; }
		inline SrscFile &getSrscFile() { return mSrscFile; }

		osg::ref_ptr<_AssetType> getAsset(RecordId assetId);


	protected:

		AssetFactory(const FilePath &containerFilePath, Database &database);

		// interface
		virtual osg::ref_ptr<_AssetType> loadAsset(RecordId id) = 0;

		// override osg::Observer
		virtual void objectDeleted(void *object) override;


	private:

		_AssetType *_getAssetFromCache(RecordId id);
		void _addAssetToCache(RecordId id, _AssetType *asset);

		Database &mDatabase;
		SrscFile mSrscFile;

		std::map<RecordId, _AssetType*> mAssetCache;
	};

	template <typename _AssetType>
	AssetFactory<_AssetType>::AssetFactory(const FilePath &containerPath, Database &database)
	: mDatabase(database)
	, mSrscFile(containerPath)
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
	        Logger::debug() << "Found " << cached->getAssetTypeName() << " " << std::hex << assetId << std::dec << " in cache";
	        return osg::ref_ptr<_AssetType>(cached);
	    }

	    osg::ref_ptr<_AssetType> loaded = this->loadAsset(assetId);
	    this->_addAssetToCache(assetId, loaded.get());

	    return loaded;
	}

	template <typename _AssetType>
	void AssetFactory<_AssetType>::objectDeleted(void *object)
	{
		_AssetType *asset = dynamic_cast<_AssetType*>(static_cast<osg::Referenced*>(object)); // a bit unsafe but osg gives us no choice
	    if(asset == nullptr)
	    {
	        Logger::warn() << "Asset factory was notified of deletion of non-asset object";
	        return;
	    }

	    RecordId assetId = asset->getAssetId();

	    Logger::debug() << "Unregistering " << asset->getAssetTypeName() << " " << std::hex << assetId << std::dec << " from cache";

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
			Logger::warn() << "Ignoring double caching of " << asset->getAssetTypeName() << " " << std::hex << assetId << std::dec;
			return;
		}

		asset->addObserver(this);

		mAssetCache[assetId] = asset;
	}
}

#endif /* INCLUDE_ASSETFACTORY_H_ */
