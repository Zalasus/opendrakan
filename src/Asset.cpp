/*
 * RiotAsset.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include "Exception.h"

#include "Asset.h"
#include "Database.h"

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



    bool AssetRef::operator<(const AssetRef &right) const
    {
        return (dbIndex << 16 | assetId) < (right.dbIndex << 16 | right.assetId);
    }

    bool AssetRef::operator==(const AssetRef &right) const
	{
    	return (dbIndex == right.dbIndex) && (assetId == right.assetId);
	}

    const AssetRef AssetRef::NULL_REF(0xffff, 0xffff);


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


