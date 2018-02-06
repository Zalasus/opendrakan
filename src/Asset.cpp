/*
 * RiotAsset.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include "Exception.h"

#include "Asset.h"

namespace od
{

    bool AssetRef::operator<(const AssetRef &right) const
    {
        return (dbIndex << 16 | assetId) < (right.dbIndex << 16 | right.assetId);
    }

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


