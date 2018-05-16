/*
 * RiotAsset.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include "db/Asset.h"

#include "db/Database.h"
#include "Exception.h"

namespace od
{

	Asset::Asset(Database &db, RecordId assetId)
	: mDb(db)
	, mId(assetId)
	{
	}

	Asset::~Asset()
	{
	}

	void Asset::loadFromRecord(DataReader &dr)
	{
	    throw UnsupportedException(std::string(this->getAssetTypeName()) + " assets can't be loaded from a single record. Need to use custom loader function and factory");
	}



    bool AssetRef::operator<(const AssetRef &right) const
    {
        return (dbIndex << 16 | assetId) < (right.dbIndex << 16 | right.assetId);
    }

    bool AssetRef::operator==(const AssetRef &right) const
	{
    	return (dbIndex == right.dbIndex) && (assetId == right.assetId);
	}

    const AssetRef AssetRef::NULL_REF(0xffff, 0xffff); // FIXME: there should be a NULL_REF and a NULL_TEXTURE_REF as one uses 0:0 and the other 0xffff:0xffff


    DataReader &operator>>(DataReader &left, AssetRef &right)
    {
        left >> right.assetId
             >> right.dbIndex;

        return left;
    }

    std::ostream &operator<<(std::ostream &left, const AssetRef &right)
    {
        left << std::hex << right.dbIndex << ":" << right.assetId << std::dec;

        return left;
    }
}


