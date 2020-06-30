/*
 * Field.h
 *
 *  Created on: 14 Oct 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_FIELD_H_
#define INCLUDE_ODCORE_RFL_FIELD_H_

#include <limits>

#include <glm/vec4.hpp>

#include <odCore/Message.h>
#include <odCore/DataStream.h>
#include <odCore/Exception.h>

namespace odRfl
{

    // TODO: with our visitor pattern FieldProbe, most of these don't have to be virtual. devirtualize them!!!

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
        inline glm::vec4 asColorVector() const { return glm::vec4(red()/255.0, green()/255.0, blue()/255.0, alpha()/255.0); }

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
    typedef EnumImpl<od::Message, 0, 9> EnumMessage;

}

#endif /* INCLUDE_ODCORE_RFL_FIELD_H_ */
