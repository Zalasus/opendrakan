/*
 * Asset.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include <odCore/db/Asset.h>

#include <odCore/db/DependencyTable.h>

namespace odDb
{

	Asset::Asset()
	: mId(0)
	{
	}

	Asset::~Asset()
	{
	}

    void Asset::setDepTableAndId(std::shared_ptr<DependencyTable> depTable, od::RecordId assetId)
    {
        mDependencyTable = depTable;
        mId = assetId;
    }

    GlobalAssetRef Asset::getGlobalAssetRef() const
    {
        return *mDependencyTable->localToGlobalRef(getLocalAssetRef());
    }

	void Asset::postLoad()
	{
	}

}
