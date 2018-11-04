/*
 * AssetRefField.h
 *
 *  Created on: 14 Oct 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_ASSETREFFIELD_H_
#define INCLUDE_ODCORE_RFL_ASSETREFFIELD_H_

#include <odCore/rfl/Field.h>
#include <odCore/db/AssetProvider.h>
#include <odCore/db/Asset.h>
#include <odCore/db/AssetProvider.h>
#include <odCore/db/Texture.h>
#include <odCore/db/Model.h>
#include <odCore/db/Class.h>
#include <odCore/db/Sequence.h>
#include <odCore/db/Animation.h>
#include <odCore/db/Sound.h>

namespace odRfl
{

    class AssetRefField : public Field
    {
    public:

        virtual ~AssetRefField() = default;

        virtual void fetchAssets(odDb::AssetProvider &ap, bool ignoreMissing = true) = 0;
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

        virtual void fetchAssets(odDb::AssetProvider &ap, bool ignoreMissing = true) override
        {
            if(mReferencedAsset == nullptr && !mReference.isNull())
            {
                try
                {
                    mReferencedAsset = ap.getAssetByRef<_AssetType>(mReference);

                }catch(od::NotFoundException &e)
                {
                    if(!ignoreMissing)
                    {
                        throw od::NotFoundException("Asset referenced by class could not be found");
                    }
                }
            }
        }

        virtual void releaseAssets() override
        {
            mReferencedAsset = nullptr;
        }

        _AssetType *getOrFetchAsset(odDb::AssetProvider &ap)
        {
            if(mReferencedAsset == nullptr)
            {
                fetchAssets(ap);
            }

            return mReferencedAsset;
        }

        _AssetType *getAsset()
        {
            return mReferencedAsset;
        }

        odDb::AssetRef getAssetRef() const { return mReference; }

    protected:

        odDb::AssetRef mReference;
        odDb::AssetPtr<_AssetType> mReferencedAsset;

    };

    typedef AssetRefFieldImpl<odDb::Class,     Field::Type::CLASS>     ClassRef;
    typedef AssetRefFieldImpl<odDb::Sound,     Field::Type::SOUND>     SoundRef;
    typedef AssetRefFieldImpl<odDb::Texture,   Field::Type::TEXTURE>   TextureRef;
    typedef AssetRefFieldImpl<odDb::Sequence,  Field::Type::SEUQUENCE> SequenceRef;
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

        virtual void fetchAssets(odDb::AssetProvider &ap, bool ignoreMissing = true) override
        {
            mReferencedAssets.reserve(mReferences.size());
            try
            {
                for(auto it = mReferences.begin(); it != mReferences.end(); ++it)
                {
                    odDb::AssetPtr<_AssetType> asset = ap.getAssetByRef<_AssetType>(*it);
                    mReferencedAssets.push_back(asset);
                }

            }catch(od::NotFoundException &e)
            {
                if(!ignoreMissing)
                {
                    throw od::NotFoundException("Asset referenced by class could not be found");
                }
            }
        }

        virtual void releaseAssets() override
        {
            for(auto it = mReferencedAssets.begin(); it != mReferencedAssets.end(); ++it)
            {
                *it = nullptr;
            }

        }

        size_t getAssetCount()
        {
            return mReferencedAssets.size();
        }

        _AssetType *getAsset(size_t i)
        {
            if(i >= mReferencedAssets.size())
            {
                throw od::Exception("Index of asset ref array access out of bounds");
            }

            return mReferencedAssets[i];
        }


    protected:

        std::vector<odDb::AssetRef> mReferences;
        std::vector<odDb::AssetPtr<_AssetType>> mReferencedAssets;

    };

    typedef AssetRefArrayFieldImpl<odDb::Sound,     Field::Type::SOUND>     SoundRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Class,     Field::Type::CLASS>     ClassRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Animation, Field::Type::ANIMATION> AnimRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Model,     Field::Type::MODEL>     ModelRefArray;

}

#endif /* INCLUDE_ODCORE_RFL_ASSETREFFIELD_H_ */
