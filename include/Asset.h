/*
 * Asset.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ASSET_H_
#define INCLUDE_ASSET_H_

#include "SrscFile.h"

namespace od
{

	class Database;

	class Asset
	{
	public:

		Asset(Database &db, RecordId assetId);
		Asset(Asset &a) = delete;
		Asset(const Asset &a) = delete;
		virtual ~Asset();

		inline RecordId getAssetId() const { return mId; }
		inline Database &getDatabase() { return mDb; }

		virtual const char *getAssetTypeName() const = 0;


	private:

		Database &mDb;
		RecordId mId;

	};

	/**
	 * Structure for representing a Record-ID & Database-Index pair as they
	 * appear all over the engines files when referencing another asset.
	 */
	struct AssetRef
	{
		AssetRef() : assetId(0), dbIndex(0) {}
		AssetRef(RecordId id, uint16_t index) : assetId(id), dbIndex(index) {}

		RecordId assetId;
		uint16_t dbIndex;

		// so we can use this in a map
		bool operator<(const AssetRef &right) const;

		bool operator==(const AssetRef &right) const;
		inline bool operator!=(const AssetRef &right) const { return !(this->operator==(right)); }

		bool isNull() const;

		static const AssetRef NULL_REF;
	};

	DataReader &operator>>(DataReader &left, AssetRef &right);

	std::ostream &operator<<(std::ostream &left, const AssetRef &right);

}

#endif /* INCLUDE_ASSET_H_ */
