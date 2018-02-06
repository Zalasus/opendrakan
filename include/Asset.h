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
	/**
	 * Structure for representing a Record-ID & Database-Index pair as they
	 * appear all over the engines files when referencing another asset.
	 */
	struct AssetRef
	{
		RecordId assetId;
		uint16_t dbIndex;

		// so we can use this in a map
		bool operator<(const AssetRef &right) const;
	};

	DataReader &operator>>(DataReader &left, AssetRef &right);

	std::ostream &operator<<(std::ostream &left, const AssetRef &right);
}

#endif /* INCLUDE_ASSET_H_ */
