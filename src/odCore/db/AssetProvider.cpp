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
    od::RefPtr<Texture> AssetProvider::getAsset<Texture>(od::RecordId id)
    {
        return std::move(this->getTexture(id));
    }

    template<>
    od::RefPtr<Class> AssetProvider::getAsset<Class>(od::RecordId id)
    {
        return std::move(this->getClass(id));
    }

    template<>
    od::RefPtr<Model> AssetProvider::getAsset<Model>(od::RecordId id)
    {
        return std::move(this->getModel(id));
    }

    template<>
    od::RefPtr<Sequence> AssetProvider::getAsset<Sequence>(od::RecordId id)
    {
        return std::move(this->getSequence(id));
    }

    template<>
    od::RefPtr<Animation> AssetProvider::getAsset<Animation>(od::RecordId id)
    {
        return std::move(this->getAnimation(id));
    }

    template<>
    od::RefPtr<Sound> AssetProvider::getAsset<Sound>(od::RecordId id)
    {
        return std::move(this->getSound(id));
    }

    od::RefPtr<Texture> AssetProvider::getTexture(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide textures");
    }

    od::RefPtr<Class> AssetProvider::getClass(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide classes");
    }

    od::RefPtr<Model> AssetProvider::getModel(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide models");
    }

    od::RefPtr<Sequence> AssetProvider::getSequence(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide sequences");
    }

    od::RefPtr<Animation> AssetProvider::getAnimation(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide animations");
    }

    od::RefPtr<Sound> AssetProvider::getSound(od::RecordId recordId)
    {
        throw od::UnsupportedException("This AssetProvider implementation can't provide sounds");
    }

}
