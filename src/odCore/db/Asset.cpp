/*
 * RiotAsset.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include <odCore/db/Asset.h>

#include <odCore/Exception.h>

namespace odDb
{

	Asset::Asset(AssetProvider &ap, od::RecordId assetId)
	: mAssetProvider(ap)
	, mId(assetId)
	{
	}

	Asset::~Asset()
	{
	}

	void Asset::load(od::SrscFile::RecordInputCursor cursor)
	{
	}

	void Asset::postLoad()
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

    const AssetRef AssetRef::NULL_REF(0, 0);

    od::DataReader &operator>>(od::DataReader &left, AssetRef &right)
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


