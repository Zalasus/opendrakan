/*
 * AssetProvider.cpp
 *
 *  Created on: 10 Apr 2018
 *      Author: zal
 */


#include <odCore/db/AssetProvider.h>

namespace od
{

    AssetProvider &AssetProvider::getDependency(uint16_t index)
    {
        throw UnsupportedException("This AssetProvider implementation can't have dependencies");
    }

    template<>
    Texture *AssetProvider::getAsset<Texture>(RecordId id)
    {
        return this->getTexture(id);
    }

    template<>
    Class *AssetProvider::getAsset<Class>(RecordId id)
    {
        return this->getClass(id);
    }

    template<>
    Model *AssetProvider::getAsset<Model>(RecordId id)
    {
        return this->getModel(id);
    }

    template<>
    Sequence *AssetProvider::getAsset<Sequence>(RecordId id)
    {
        return this->getSequence(id);
    }

    template<>
    Animation *AssetProvider::getAsset<Animation>(RecordId id)
    {
        return this->getAnimation(id);
    }

    template<>
    Sound *AssetProvider::getAsset<Sound>(RecordId id)
    {
        return this->getSound(id);
    }

    Texture *AssetProvider::getTexture(RecordId recordId)
    {
        throw UnsupportedException("This AssetProvider implementation can't provide textures");
    }

    Class *AssetProvider::getClass(RecordId recordId)
    {
        throw UnsupportedException("This AssetProvider implementation can't provide classes");
    }

    Model *AssetProvider::getModel(RecordId recordId)
    {
        throw UnsupportedException("This AssetProvider implementation can't provide models");
    }

    Sequence *AssetProvider::getSequence(RecordId recordId)
    {
        throw UnsupportedException("This AssetProvider implementation can't provide sequences");
    }

    Animation *AssetProvider::getAnimation(RecordId recordId)
    {
        throw UnsupportedException("This AssetProvider implementation can't provide animations");
    }

    Sound *AssetProvider::getSound(RecordId recordId)
    {
        throw UnsupportedException("This AssetProvider implementation can't provide sounds");
    }

}
