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
	, mReferenceCount(0)
	, mObserver(nullptr)
	{
	}

	Asset::~Asset()
	{
	    if(mReferenceCount > 0)
	    {
	        Logger::warn() << "Asset destroyed when referenced to it still existed";
	    }
	}

	void Asset::referenceCreated()
	{
	    ++mReferenceCount;
	}

    void Asset::referenceDestroyed()
    {
        if(mReferenceCount == 0)
        {
            throw od::Exception("Called referenceDestroyed() on Asset with refcount = 0");
        }

        --mReferenceCount;
    }

    void Asset::referenceReleased()
    {
        if(mReferenceCount > 0)
        {
            --mReferenceCount;
        }
    }

    void Asset::setReferenceObserver(AssetReferenceObserver *observer)
    {
        if(mObserver != nullptr)
        {
            Logger::warn() << "Assigning observer to Asset which already had an observer";
        }

        mObserver = observer;
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


