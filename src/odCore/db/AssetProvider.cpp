/*
 * AssetProvider.cpp
 *
 *  Created on: 10 Apr 2018
 *      Author: zal
 */


#include <odCore/db/AssetProvider.h>

namespace odDb
{

    AssetProvider &AssetProvider::getDependency(uint16_t index)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't have dependencies");
    }

    template<>
    Texture *AssetProvider::getAsset<Texture>(od::RecordId id)
    {
        return this->getTexture(id);
    }

    template<>
    Class *AssetProvider::getAsset<Class>(od::RecordId id)
    {
        return this->getClass(id);
    }

    template<>
    Model *AssetProvider::getAsset<Model>(od::RecordId id)
    {
        return this->getModel(id);
    }

    template<>
    Sequence *AssetProvider::getAsset<Sequence>(od::RecordId id)
    {
        return this->getSequence(id);
    }

    template<>
    Animation *AssetProvider::getAsset<Animation>(od::RecordId id)
    {
        return this->getAnimation(id);
    }

    template<>
    Sound *AssetProvider::getAsset<Sound>(od::RecordId id)
    {
        return this->getSound(id);
    }

    Texture *AssetProvider::getTexture(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide textures");
    }

    Class *AssetProvider::getClass(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide classes");
    }

    Model *AssetProvider::getModel(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide models");
    }

    Sequence *AssetProvider::getSequence(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide sequences");
    }

    Animation *AssetProvider::getAnimation(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide animations");
    }

    Sound *AssetProvider::getSound(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide sounds");
    }

}
