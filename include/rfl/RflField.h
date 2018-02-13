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

#include "Asset.h"

namespace od
{
    class RflFieldProbe;

	/**
	 * Common parent to all RflFields so RflClassBuilder can store them in a single vector.
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

		RflField(RflFieldProbe &probe, RflFieldType type, const char *fieldName);
		RflField(const RflField &f) = delete;
		RflField(RflField &f) = delete;
		virtual ~RflField() {}

		virtual bool isArray() const = 0;

		virtual void fill(DataReader &dr);
		virtual void fillArray(uint16_t size, DataReader &dr);
	};

	class RflString : public RflField
	{
	public:

		RflString(RflFieldProbe &probe, const char *name, const std::string &defaultValue);

		virtual bool isArray() const override { return true; }

		virtual void fillArray(uint16_t size, DataReader &dr) override;


	private:

		std::string mValue;
	};



	template <RflField::RflFieldType _Type>
	class RflAssetRef : public RflField
	{
	public:

		RflAssetRef(RflFieldProbe &probe, const char *name, const AssetRef &defaultValue)
		: RflField(probe, _Type, name)
		, mValue(defaultValue)
		{
		}

		virtual bool isArray() const override { return false; }

		virtual void fill(DataReader &dr) override
		{
			dr >> mValue;
		}


	private:

		AssetRef mValue;

	};

	typedef RflAssetRef<RflField::CLASS>     RflClassRef;
	typedef RflAssetRef<RflField::SOUND>     RflSoundRef;
	typedef RflAssetRef<RflField::SEUQUENCE> RflSequenceRef;



	template <typename _DataType, RflField::RflFieldType _Type>
	class RflPOD : public RflField
	{
	public:

		RflPOD(RflFieldProbe &probe, const char *name, const _DataType &defaultValue)
	    : RflField(probe, _Type, name)
		, mValue(defaultValue)
		{
		}

		virtual bool isArray() const override { return false; }

		virtual void fill(DataReader &dr) override
		{
			dr >> mValue;
		}


	private:

		_DataType mValue;

	};

	typedef RflPOD< int32_t, RflField::INTEGER> RflInteger;
	typedef RflPOD<   float,   RflField::FLOAT> RflFloat;
	typedef RflPOD<uint32_t,    RflField::ENUM> RflEnum;
	typedef RflEnum                             RflEnumYesNo; // give these a list of allowed values once we implement that feature
	typedef RflEnum                             RflEnumPlayerSlot;
}

#endif /* INCLUDE_RFL_RFLFIELD_H_ */
