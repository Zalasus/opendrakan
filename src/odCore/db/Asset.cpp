/*
 * Asset.cpp
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

	void Asset::postLoad()
	{
	}

}
