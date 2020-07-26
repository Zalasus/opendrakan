/*
 * AssetProvider.cpp
 *
 *  Created on: 10 Apr 2018
 *      Author: zal
 */


#include <odCore/db/AssetProvider.h>

#include <odCore/db/TextureFactory.h>
#include <odCore/db/ModelFactory.h>
#include <odCore/db/ClassFactory.h>
#include <odCore/db/AnimationFactory.h>
#include <odCore/db/SoundFactory.h>
#include <odCore/db/SequenceFactory.h>

namespace odDb
{

    AssetProvider &AssetProvider::getDependency(uint16_t index)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't have dependencies");
    }

    template<>
    std::shared_ptr<Texture> AssetProvider::getAsset<Texture>(od::RecordId id)
    {
        return this->getTexture(id);
    }

    template<>
    std::shared_ptr<Class> AssetProvider::getAsset<Class>(od::RecordId id)
    {
        return this->getClass(id);
    }

    template<>
    std::shared_ptr<Model> AssetProvider::getAsset<Model>(od::RecordId id)
    {
        return this->getModel(id);
    }

    template<>
    std::shared_ptr<Sequence> AssetProvider::getAsset<Sequence>(od::RecordId id)
    {
        return this->getSequence(id);
    }

    template<>
    std::shared_ptr<Animation> AssetProvider::getAsset<Animation>(od::RecordId id)
    {
        return this->getAnimation(id);
    }

    template<>
    std::shared_ptr<Sound> AssetProvider::getAsset<Sound>(od::RecordId id)
    {
        return this->getSound(id);
    }

    std::shared_ptr<Texture> AssetProvider::getTexture(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide textures");
    }

    std::shared_ptr<Class> AssetProvider::getClass(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide classes");
    }

    std::shared_ptr<Model> AssetProvider::getModel(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide models");
    }

    std::shared_ptr<Sequence> AssetProvider::getSequence(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide sequences");
    }

    std::shared_ptr<Animation> AssetProvider::getAnimation(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide animations");
    }

    std::shared_ptr<Sound> AssetProvider::getSound(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide sounds");
    }

}
