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
#include <osg/ref_ptr>

#include "db/Asset.h"
#include "db/AssetProvider.h"
#include "db/Animation.h"

namespace odRfl
{
    class RflFieldProbe;

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
		virtual RflFieldType getFieldType() const { return STRING; }

		virtual void fillArray(uint16_t size, od::DataReader &dr) override;


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

	template <typename _AssetType, RflField::RflFieldType _Type>
	class RflAssetRef : public RflPOD<od::AssetRef, _Type>
	{
	public:

	    RflAssetRef()
	    : RflPOD<od::AssetRef, _Type>(od::AssetRef::NULL_REF)
        {
        }

	    RflAssetRef(const od::AssetRef &ref)
        : RflPOD<od::AssetRef, _Type>(ref)
        {
        }

        _AssetType *getOrFetchAsset(od::AssetProvider &ap)
        {
            if(mReferencedAsset == nullptr)
            {
                try
                {
                    mReferencedAsset = ap.getAssetByRef<_AssetType>(this->get());

                }catch(od::NotFoundException &e)
                {
                    throw od::NotFoundException("Asset referenced by class could not be found");
                }
            }

            return mReferencedAsset;
        }

        _AssetType *getAsset()
        {
            return mReferencedAsset;
        }

	protected:

	    osg::ref_ptr<_AssetType> mReferencedAsset;

	};

    typedef RflPOD<od::AssetRef, RflField::CLASS>     RflClassRef;
    typedef RflPOD<od::AssetRef, RflField::SOUND>     RflSoundRef;
    typedef RflPOD<od::AssetRef, RflField::TEXTURE>   RflTextureRef;
    typedef RflPOD<od::AssetRef, RflField::SEUQUENCE> RflSequenceRef;
    typedef RflAssetRef<od::Animation, RflField::ANIMATION> RflAnimRef;


	typedef RflPOD< int32_t, RflField::INTEGER> RflInteger;
	typedef RflPOD<   float,   RflField::FLOAT> RflFloat;
	typedef RflPOD<uint32_t,    RflField::ENUM> RflEnum;
	typedef RflEnum                             RflEnumYesNo; // TODO: give these a list of allowed values once we implement that feature
	typedef RflEnum                             RflEnumPlayerSlot;
	typedef RflEnum 							RflEnumMessage;
	typedef RflPOD<uint32_t, RflField::CHAR_CHANNEL> RflCharChannel;


	template <typename _DataType, RflField::RflFieldType _Type>
	class RflPODArray : public RflField
	{
	public:

		RflPODArray(const std::initializer_list<_DataType> defaultValues)
	    : mValues(defaultValues)
		{
		}

		virtual bool isArray() const override { return true; }
		virtual RflFieldType getFieldType() const { return _Type; }

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

	typedef RflPODArray<od::AssetRef, RflField::SOUND> RflSoundRefArray;
	typedef RflPODArray<od::AssetRef, RflField::CLASS> RflClassRefArray;
	typedef RflPODArray<od::AssetRef, RflField::ANIMATION> RflAnimRefArray;
	typedef RflPODArray<od::AssetRef, RflField::MODEL> RflModelRefArray;
	typedef RflPODArray<uint32_t, RflField::CHAR_CHANNEL> RflCharChannelArray;
}

#endif /* INCLUDE_RFL_RFLFIELD_H_ */
