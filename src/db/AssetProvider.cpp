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
    Texture *AssetProvider::getAssetByRef<Texture>(const AssetRef &ref)
    {
        return this->getTextureByRef(ref);
    }

    template<>
    Class *AssetProvider::getAssetByRef<Class>(const AssetRef &ref)
    {
        return this->getClassByRef(ref);
    }

    template<>
    Model *AssetProvider::getAssetByRef<Model>(const AssetRef &ref)
    {
        return this->getModelByRef(ref);
    }

    template<>
    Sequence *AssetProvider::getAssetByRef<Sequence>(const AssetRef &ref)
    {
        return this->getSequenceByRef(ref);
    }

    template<>
    Animation *AssetProvider::getAssetByRef<Animation>(const AssetRef &ref)
    {
        return this->getAnimationByRef(ref);
    }

    template<>
    Sound *AssetProvider::getAssetByRef<Sound>(const AssetRef &ref)
    {
        return this->getSoundByRef(ref);
    }

}
