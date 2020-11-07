/*
 * Asset.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ASSET_H_
#define INCLUDE_ASSET_H_

#include <odCore/SrscFile.h>

#include <odCore/db/AssetRef.h>

namespace odDb
{

	class AssetProvider;

	class Asset
	{
	public:

		Asset(AssetProvider &ap, od::RecordId assetId);
		Asset(Asset &a) = delete;
		Asset(const Asset &a) = delete;
		virtual ~Asset();

		inline od::RecordId getAssetId() const { return mId; }
		inline AssetProvider &getAssetProvider() { return mAssetProvider; };

		/**
		 * Implemented by an asset to facilitate loading from a record.
		 *
		 * Until this returns, the asset container is locked and only accessible to this method
		 * via the cursor provided. It is not possible to load an asset from the same container
		 * until the file is unlocked again.
		 *
		 * If you need your asset to load another asset from the same container (like for texture animations),
		 * store the IDs you want to load in this method and actually load them in the postLoad() method, which
		 * is called when the file is unlocked again.
		 */
		virtual void load(od::SrscFile::RecordInputCursor cursor) = 0;

		/**
		 * Called after load() returned and the asset container is unlocked again.
		 *
		 * Does nothing by default.
		 */
		virtual void postLoad();


	private:

		AssetProvider &mAssetProvider;
		od::RecordId mId;

	};

	template <typename T>
	struct AssetTraits
	{
	    static const char *name();
	    static constexpr od::RecordType baseType();
	};

}

#endif /* INCLUDE_ASSET_H_ */
