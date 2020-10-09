/*
 * RiotAsset.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include <odCore/db/Asset.h>

#include <functional>

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

	void Asset::postLoad()
	{
	}


    bool AssetRef::operator==(const AssetRef &right) const
	{
    	return (dbIndex == right.dbIndex) && (assetId == right.assetId);
	}

    const AssetRef AssetRef::NULL_REF(0, 0);


    size_t AssetRefHasher::operator()(const AssetRef &ref) const
    {
        std::hash<uint32_t> hash;
        uint32_t intRepresentation = (ref.assetId << 16) | ref.dbIndex;
        return hash(intRepresentation);
    }


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
