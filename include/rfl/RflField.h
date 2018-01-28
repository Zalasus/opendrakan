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
	enum RflFieldType
	{
		INTEGER 		= 0x01,
		FLOAT   		= 0x02,
		CLASS   		= 0x03,
		MODEL			= 0x04,
		SOUND			= 0x05,
		ENUM			= 0x07,
		CHAR_CHANNEL 	= 0x08,
		ANIMATION 		= 0x09,
		STRING			= 0x0a,
		SEUQUENCE		= 0x0b,
		TEXTURE			= 0x0e,
		COLOR			= 0x0f
	};

	/**
	 * Common parent to all RflFields so RflClassBuilder can store them in a single vector.
	 */
	class RflField
	{
	public:

		RflField(RflClassBuilder &cb);

	};

	template <typename _DataType, typename _FieldType>
	class RflFieldImpl : public RflField
	{
	public:

		RflFieldImpl(RflClassBuilder &cb, char *name, _DataType defaultValue, RflFieldType fieldType)
		: RflField(cb)
		, mFieldName(name)
		, mDefaultValue(defaultValue)
		, mFieldType(fieldType)
		{
		}

		inline _DataType getDefaultValue() const { return mDefaultValue; };

	private:

		std::string mFieldName;
		_DataType mDefaultValue;
		RflFieldType mFieldType;

	};

	class RflString : public RflFieldImpl<std::string, RflString>
	{
	public:

		RflString(RflClassBuilder &cb, char *name, const std::string &defaultValue)
		: RflFieldImpl<std::string, RflString>(cb, name, defaultValue, STRING)
		{
		}
	};

	class RflInteger : public RflFieldImpl<int32_t, RflInteger>
	{
	public:

		RflInteger(RflClassBuilder &cb, char *name, int32_t defaultValue)
		: RflFieldImpl<int32_t, RflInteger>(cb, name, defaultValue, INTEGER)
		{

		}
	};

	class RflEnum : public RflFieldImpl<uint32_t, RflEnum>
	{
		RflEnum(RflClassBuilder &cb, char *name, int32_t defaultValue, std::initializer_list<uint32_t> possibleItems)
		: RflFieldImpl<uint32_t, RflEnum>(cb, name, defaultValue, ENUM)
		{

		}
	};

}

#endif /* INCLUDE_RFL_RFLFIELD_H_ */
