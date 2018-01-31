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

		RflField(RflFieldProbe *probe, RflFieldType type, const char *fieldName);

	};

	template <typename _DataType, typename _FieldType>
	class RflFieldImpl : public RflField
	{
	public:

		RflFieldImpl(RflFieldProbe *probe, const char *name, _DataType defaultValue, RflFieldType fieldType)
		: RflField(probe, fieldType, name)
		, mValue(defaultValue)
		{
		}

		inline _DataType getValue() const { return mValue; };

	private:

		_DataType mValue;
	};

	class RflString : public RflFieldImpl<std::string, RflString>
	{
	public:

		RflString(RflFieldProbe *probe, const char *name, const std::string &defaultValue)
		: RflFieldImpl<std::string, RflString>(probe, name, defaultValue, STRING)
		{
		}
	};

	class RflInteger : public RflFieldImpl<int32_t, RflInteger>
	{
	public:

		RflInteger(RflFieldProbe *probe, const char *name, int32_t defaultValue)
		: RflFieldImpl<int32_t, RflInteger>(probe, name, defaultValue, INTEGER)
		{

		}
	};

}

#endif /* INCLUDE_RFL_RFLFIELD_H_ */
