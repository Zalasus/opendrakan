/*
 * AssetRefField.h
 *
 *  Created on: 14 Oct 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_ASSETREFFIELD_H_
#define INCLUDE_ODCORE_RFL_ASSETREFFIELD_H_

#include <odCore/rfl/Field.h>

#include <odCore/db/Asset.h>
#include <odCore/db/Texture.h>
#include <odCore/db/Model.h>
#include <odCore/db/Class.h>
#include <odCore/db/Sequence.h>
#include <odCore/db/Animation.h>
#include <odCore/db/Sound.h>
#include <odCore/db/DependencyTable.h>

namespace odRfl
{

    class AssetRefField : public Field
    {
    public:

        virtual ~AssetRefField() = default;

        virtual bool fetchAssets(const odDb::DependencyTable &dt) = 0;
        virtual void releaseAssets() = 0;
    };

    template <typename _AssetType, Field::Type _FieldType>
    class AssetRefFieldImpl : public AssetRefField
    {
    public:

        AssetRefFieldImpl(const odDb::AssetRef &ref)
        : mReference(ref)
        {
        }

        virtual bool isArray() const override { return false; }
        virtual Field::Type getFieldType() const override { return _FieldType; }

        virtual void fill(od::DataReader &dr) override
        {
            dr >> mReference;
        }

        virtual bool fetchAssets(const odDb::DependencyTable &dt) override
        {
            if(mReferencedAsset == nullptr && !mReference.isNull())
            {
                mReferencedAsset = dt.loadAsset<_AssetType>(mReference);
            }

            return mReferencedAsset == nullptr;
        }

        virtual void releaseAssets() override
        {
            mReferencedAsset = nullptr;
        }

        std::shared_ptr<_AssetType> getOrFetchAsset(odDb::DependencyTable &dt)
        {
            if(mReferencedAsset == nullptr)
            {
                fetchAssets(dt);
            }

            return mReferencedAsset;
        }

        std::shared_ptr<_AssetType> getAsset() const
        {
            return mReferencedAsset;
        }

        odDb::AssetRef getAssetRef() const { return mReference; }


    protected:

        odDb::AssetRef mReference;
        std::shared_ptr<_AssetType> mReferencedAsset;

    };

    typedef AssetRefFieldImpl<odDb::Class,     Field::Type::CLASS>     ClassRef;
    typedef AssetRefFieldImpl<odDb::Sound,     Field::Type::SOUND>     SoundRef;
    typedef AssetRefFieldImpl<odDb::Texture,   Field::Type::TEXTURE>   TextureRef;
    typedef AssetRefFieldImpl<odDb::Sequence,  Field::Type::SEQUENCE> SequenceRef;
    typedef AssetRefFieldImpl<odDb::Animation, Field::Type::ANIMATION> AnimRef;
    typedef AssetRefFieldImpl<odDb::Model,     Field::Type::MODEL>     ModelRef;



    template <typename _AssetType, Field::Type _FieldType>
    class AssetRefArrayFieldImpl : public AssetRefField
    {
    public:

        AssetRefArrayFieldImpl()
        {
        }

        AssetRefArrayFieldImpl(const std::initializer_list<odDb::AssetRef> defaultRefs)
        : mReferences(defaultRefs)
        {
        }

        virtual bool isArray() const override { return true; }
        virtual Field::Type getFieldType() const override { return _FieldType; }

        virtual void fillArray(uint16_t size, od::DataReader &dr) override
        {
            mReferences.clear();
            mReferences.reserve(size);
            for(size_t i = 0; i < size; ++i)
            {
                odDb::AssetRef v;
                dr >> v;
                mReferences.push_back(v);
            }
            mReferences.shrink_to_fit();
        }

        virtual bool fetchAssets(const odDb::DependencyTable &dt) override
        {
            mReferencedAssets.reserve(mReferences.size());

            bool fetchedAll = true;
            for(auto &ref : mReferences)
            {
                auto asset = dt.loadAsset<_AssetType>(ref);
                if(asset == nullptr)
                {
                    fetchedAll = false;
                }

                mReferencedAssets.push_back(asset);
            }

            return fetchedAll;
        }

        virtual void releaseAssets() override
        {
            for(auto &asset : mReferencedAssets)
            {
                asset = nullptr;
            }
        }

        size_t getAssetCount()
        {
            return mReferencedAssets.size();
        }

        std::shared_ptr<_AssetType> getAsset(size_t i)
        {
            if(i >= mReferencedAssets.size())
            {
                OD_PANIC() << "Index of asset array access out of bounds: index=" << i << " size=" << mReferencedAssets.size();
            }

            return mReferencedAssets[i];
        }

        odDb::AssetRef getAssetRef(size_t i)
        {
            if(i >= mReferences.size())
            {
                OD_PANIC() << "Index of asset ref array access out of bounds: index=" << i << " size=" << mReferences.size();
            }

            return mReferences[i];
        }


    protected:

        std::vector<odDb::AssetRef> mReferences;
        std::vector<std::shared_ptr<_AssetType>> mReferencedAssets;

    };

    typedef AssetRefArrayFieldImpl<odDb::Sequence,  Field::Type::SEQUENCE>  SequenceRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Sound,     Field::Type::SOUND>     SoundRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Class,     Field::Type::CLASS>     ClassRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Animation, Field::Type::ANIMATION> AnimRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Model,     Field::Type::MODEL>     ModelRefArray;

}

#endif /* INCLUDE_ODCORE_RFL_ASSETREFFIELD_H_ */
