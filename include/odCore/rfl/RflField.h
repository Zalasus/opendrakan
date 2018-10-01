/*
 * RflField.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLFIELD_H_
#define INCLUDE_RFL_RFLFIELD_H_

#include <initializer_list>
#include <string>
#include <cstring>
#include <limits>
#include <osg/ref_ptr>

#include <odCore/db/Asset.h>
#include <odCore/db/AssetProvider.h>
#include <odCore/db/Texture.h>
#include <odCore/db/Model.h>
#include <odCore/db/Class.h>
#include <odCore/db/Sequence.h>
#include <odCore/db/Animation.h>
#include <odCore/db/Sound.h>
#include <odCore/rfl/RflMessage.h>

namespace od
{

	/**
	 * Common parent to all RflFields so RflClassBuilder/RflFieldProbe can store them in a single vector.
	 */
	class RflField
	{
	public:

        enum RflFieldType
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

        RflField() = default;
		RflField(const RflField &f) = delete;
		RflField(RflField &f) = delete;
		virtual ~RflField() = default;

		virtual bool isArray() const = 0;
		virtual RflFieldType getFieldType() const = 0;

		virtual void fill(od::DataReader &dr);
		virtual void fillArray(uint16_t size, od::DataReader &dr);
	};



	class RflString : public RflField
	{
	public:

		RflString(const std::string &defaultValue);

		virtual bool isArray() const override { return true; }
		RflFieldType getFieldType() const override { return STRING; }

		virtual void fillArray(uint16_t size, od::DataReader &dr) override;

		const std::string &get() { return mValue; }
		operator std::string () const { return mValue; }


	private:

		std::string mValue;
	};



	template <typename _DataType, RflField::RflFieldType _Type>
	class RflPOD : public RflField
	{
	public:

		RflPOD(const _DataType &defaultValue)
	    : mValue(defaultValue)
		{
		}

		virtual bool isArray() const override { return false; }
		virtual RflFieldType getFieldType() const override { return _Type; }

		virtual void fill(od::DataReader &dr) override
		{
			dr >> mValue;
		}

		_DataType get() const { return mValue; }
		operator _DataType () const { return mValue; }


	protected:

		_DataType mValue;

	};

    typedef RflPOD< int32_t, RflField::INTEGER>      RflInteger;
    typedef RflPOD<   float,   RflField::FLOAT>      RflFloat;
    typedef RflPOD<uint32_t, RflField::CHAR_CHANNEL> RflCharChannel;

    class RflColor : public RflPOD<uint32_t, RflField::COLOR>
    {
    public:

        RflColor(uint32_t i) : RflPOD<uint32_t, RflField::COLOR>(i) { }

        inline uint8_t red() const { return (mValue >> 16) & 0xff; }
        inline uint8_t green() const { return (mValue >> 8) & 0xff; }
        inline uint8_t blue() const { return mValue & 0xff; }
        inline uint8_t alpha() const { return (mValue >> 24) & 0xff; }
        inline osg::Vec4 asColorVector() const { return osg::Vec4(red()/255.0, green()/255.0, blue()/255.0, alpha()/255.0); }

    };


	template <typename _EnumType, uint32_t _MinValue = std::numeric_limits<uint32_t>::min(), uint32_t _MaxValue = std::numeric_limits<uint32_t>::max()>
    class RflEnumImpl : public RflField
    {
    public:

        RflEnumImpl(_EnumType defaultValue)
        : mValue(defaultValue)
        {
        }

        virtual bool isArray() const override { return false; }
        virtual RflFieldType getFieldType() const override { return RflField::ENUM; }

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

	typedef RflEnumImpl<uint32_t> RflEnum;

    typedef RflEnumImpl<bool, 0, 1> RflEnumYesNo;

    typedef RflEnumImpl<RflMessage, 0, 9> RflEnumMessage;


    class RflAssetRef : public RflField
    {
    public:

        virtual ~RflAssetRef() = default;

        virtual void fetchAssets(od::AssetProvider &ap, bool ignoreMissing = true) = 0;
        virtual void releaseAssets() = 0;

    };

	template <typename _AssetType, RflField::RflFieldType _FieldType>
	class RflAssetRefImpl : public RflAssetRef
	{
	public:

	    RflAssetRefImpl(const od::AssetRef &ref)
	    : mReference(ref)
        {
        }

	    virtual bool isArray() const override { return false; }
        virtual RflFieldType getFieldType() const override { return _FieldType; }

        virtual void fill(od::DataReader &dr) override
        {
            dr >> mReference;
        }

	    virtual void fetchAssets(od::AssetProvider &ap, bool ignoreMissing = true) override
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

        _AssetType *getOrFetchAsset(od::AssetProvider &ap)
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

        od::AssetRef getAssetRef() const { return mReference; }

	protected:

        od::AssetRef mReference;
	    osg::ref_ptr<_AssetType> mReferencedAsset;

	};

    typedef RflAssetRefImpl<od::Class, RflField::CLASS>         RflClassRef;
    typedef RflAssetRefImpl<od::Sound, RflField::SOUND>         RflSoundRef;
    typedef RflAssetRefImpl<od::Texture, RflField::TEXTURE>     RflTextureRef;
    typedef RflAssetRefImpl<od::Sequence, RflField::SEUQUENCE>  RflSequenceRef;
    typedef RflAssetRefImpl<od::Animation, RflField::ANIMATION> RflAnimRef;
    typedef RflAssetRefImpl<od::Model, RflField::MODEL>         RflModelRef;



    template <typename _AssetType, RflField::RflFieldType _FieldType>
    class RflAssetRefArrayImpl : public RflAssetRef
    {
    public:

        RflAssetRefArrayImpl()
        {
        }

        RflAssetRefArrayImpl(const std::initializer_list<od::AssetRef> defaultRefs)
        : mReferences(defaultRefs)
        {
        }

        virtual bool isArray() const override { return true; }
        virtual RflFieldType getFieldType() const override { return _FieldType; }

        virtual void fillArray(uint16_t size, od::DataReader &dr) override
        {
            mReferences.clear();
            mReferences.reserve(size);
            for(size_t i = 0; i < size; ++i)
            {
                od::AssetRef v;
                dr >> v;
                mReferences.push_back(v);
            }
            mReferences.shrink_to_fit();
        }

        virtual void fetchAssets(od::AssetProvider &ap, bool ignoreMissing = true) override
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

        std::vector<od::AssetRef> mReferences;
        std::vector<osg::ref_ptr<_AssetType>> mReferencedAssets;

    };

    typedef RflAssetRefArrayImpl<od::Sound, RflField::SOUND>            RflSoundRefArray;
    typedef RflAssetRefArrayImpl<od::Class, RflField::CLASS>            RflClassRefArray;
    typedef RflAssetRefArrayImpl<od::Animation, RflField::ANIMATION>    RflAnimRefArray;
    typedef RflAssetRefArrayImpl<od::Model, RflField::MODEL>            RflModelRefArray;



	template <typename _DataType, RflField::RflFieldType _Type>
	class RflPODArray : public RflField
	{
	public:

		RflPODArray(const std::initializer_list<_DataType> defaultValues)
	    : mValues(defaultValues)
		{
		}

		virtual bool isArray() const override { return true; }
		RflFieldType getFieldType() const override { return _Type; }

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

	typedef RflPODArray<uint32_t, RflField::CHAR_CHANNEL> RflCharChannelArray;

}

#endif /* INCLUDE_RFL_RFLFIELD_H_ */
