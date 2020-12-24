/*
 * Asset.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ASSET_H_
#define INCLUDE_ASSET_H_

#include <memory>

#include <odCore/SrscFile.h>

#include <odCore/db/AssetRef.h>

namespace odDb
{

    class DependencyTable;

	class Asset
	{
	public:

		Asset();
		Asset(const Asset &a) = delete;
		virtual ~Asset();

        inline AssetRef getLocalAssetRef() const { return AssetRef(mId, AssetRef::SELF_DBINDEX); }
		inline od::RecordId getAssetId() const { return mId; }
        inline std::shared_ptr<DependencyTable> getDependencyTable() { return mDependencyTable; }

        /**
         * @brief Only to be used by AssetFactory during loading.
         *
         * We pass these values through a setter to reduce redundant constructors in asset types.
         */
        void setDepTableAndId(std::shared_ptr<DependencyTable> depTable, od::RecordId assetId);

        GlobalAssetRef getGlobalAssetRef() const;

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

        std::shared_ptr<DependencyTable> mDependencyTable;
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
