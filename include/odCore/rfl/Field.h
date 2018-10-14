/*
 * Field.h
 *
 *  Created on: 14 Oct 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_FIELD_H_
#define INCLUDE_ODCORE_RFL_FIELD_H_

#include <limits>
#include <osg/Vec4>
#include <osg/ref_ptr>

#include <odCore/rfl/RflMessage.h>
#include <odCore/db/AssetProvider.h>
#include <odCore/DataStream.h>

namespace odRfl
{

    /**
     * Common parent to all Fields.
     */
    class Field
    {
    public:

        enum class Type
        {
            INTEGER         = 0x01,
            FLOAT           = 0x02,
            CLASS           = 0x03,
            MODEL           = 0x04,
            SOUND           = 0x05,
            ENUM            = 0x07,
            CHAR_CHANNEL    = 0x08,
            ANIMATION       = 0x09,
            STRING          = 0x0a,
            SEUQUENCE       = 0x0b,
            TEXTURE         = 0x0e,
            COLOR           = 0x0f
        };

        Field() = default;
        Field(const Field &f) = delete;
        Field(Field &f) = delete;
        virtual ~Field() = default;

        virtual bool isArray() const = 0;
        virtual Type getFieldType() const = 0;

        virtual void fill(od::DataReader &dr);
        virtual void fillArray(uint16_t size, od::DataReader &dr);
    };



    class String : public Field
    {
    public:

        String(const std::string &defaultValue);

        virtual bool isArray() const override { return true; }
        Field::Type getFieldType() const override { return Field::Type::STRING; }

        virtual void fillArray(uint16_t size, od::DataReader &dr) override;

        const std::string &get() { return mValue; }
        operator std::string () const { return mValue; }


    private:

        std::string mValue;
    };



    template <typename _DataType, Field::Type _Type>
    class PodField : public Field
    {
    public:

        PodField(const _DataType &defaultValue)
        : mValue(defaultValue)
        {
        }

        virtual bool isArray() const override { return false; }
        virtual Field::Type getFieldType() const override { return _Type; }

        virtual void fill(od::DataReader &dr) override
        {
            dr >> mValue;
        }

        _DataType get() const { return mValue; }
        operator _DataType () const { return mValue; }


    protected:

        _DataType mValue;

    };

    typedef PodField< int32_t, Field::Type::INTEGER>      Integer;
    typedef PodField<   float, Field::Type::FLOAT>        Float;
    typedef PodField<uint32_t, Field::Type::CHAR_CHANNEL> CharChannel;


    class Color : public PodField<uint32_t, Field::Type::COLOR>
    {
    public:

        using PodField<uint32_t, Field::Type::COLOR>::PodField;

        inline uint8_t red() const { return (mValue >> 16) & 0xff; }
        inline uint8_t green() const { return (mValue >> 8) & 0xff; }
        inline uint8_t blue() const { return mValue & 0xff; }
        inline uint8_t alpha() const { return (mValue >> 24) & 0xff; }
        inline osg::Vec4 asColorVector() const { return osg::Vec4(red()/255.0, green()/255.0, blue()/255.0, alpha()/255.0); }

    };


    template <typename _DataType, Field::Type _Type>
    class PodArray : public Field
    {
    public:

        PodArray(const std::initializer_list<_DataType> defaultValues)
        : mValues(defaultValues)
        {
        }

        virtual bool isArray() const override { return true; }
        Field::Type getFieldType() const override { return _Type; }

        virtual void fillArray(uint16_t size, od::DataReader &dr) override
        {
            mValues.clear();
            mValues.reserve(size);
            for(size_t i = 0; i < size; ++i)
            {
                _DataType v;
                dr >> v;
                mValues.push_back(v);
            }
            mValues.shrink_to_fit();
        }


    private:

        std::vector<_DataType> mValues;

    };

    typedef PodArray<uint32_t, Field::Type::CHAR_CHANNEL> CharChannelArray;


    template <typename _EnumType, uint32_t _MinValue = std::numeric_limits<uint32_t>::min(), uint32_t _MaxValue = std::numeric_limits<uint32_t>::max()>
    class EnumImpl : public Field
    {
    public:

        EnumImpl(_EnumType defaultValue)
        : mValue(defaultValue)
        {
        }

        virtual bool isArray() const override { return false; }
        virtual Field::Type getFieldType() const override { return Field::Type::ENUM; }

        virtual void fill(od::DataReader &dr) override
        {
            uint32_t value;
            dr >> value;

            if(value < _MinValue || value > _MaxValue)
            {
                // FIXME: is this really helpful? i don't think so
                Logger::warn() << "Enum value in RFL class exceeds that enum's allowed value range";
            }

            mValue = static_cast<_EnumType>(value);
        }

        _EnumType get() const { return mValue; }
        operator _EnumType () const { return mValue; }


    protected:

        _EnumType mValue;

    };

    typedef EnumImpl<uint32_t>          Enum;
    typedef EnumImpl<bool, 0, 1>        EnumYesNo;
    typedef EnumImpl<RflMessage, 0, 9>  EnumMessage;


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
        osg::ref_ptr<_AssetType> mReferencedAsset;

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
                    osg::ref_ptr<_AssetType> asset = ap.getAssetByRef<_AssetType>(*it);
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
        std::vector<osg::ref_ptr<_AssetType>> mReferencedAssets;

    };

    typedef AssetRefArrayFieldImpl<odDb::Sound,     Field::Type::SOUND>     SoundRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Class,     Field::Type::CLASS>     ClassRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Animation, Field::Type::ANIMATION> AnimRefArray;
    typedef AssetRefArrayFieldImpl<odDb::Model,     Field::Type::MODEL>     ModelRefArray;

}

#endif /* INCLUDE_ODCORE_RFL_FIELD_H_ */
