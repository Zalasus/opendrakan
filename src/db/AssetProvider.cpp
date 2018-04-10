/*
 * AssetProvider.cpp
 *
 *  Created on: 10 Apr 2018
 *      Author: zal
 */


#include "db/AssetProvider.h"

namespace od
{

    template<>
    Animation *AssetProvider::getAssetByRef<Animation>(const AssetRef &ref)
    {
        return this->getAnimationByRef(ref);
    }

}
