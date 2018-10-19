/*
 * ObjectBuilderProbe.cpp
 *
 *  Created on: 19 Oct 2018
 *      Author: zal
 */

#include <odCore/rfl/ObjectBuilderProbe.h>

#include <algorithm>

#include <odCore/Exception.h>
#include <odCore/rfl/Field.h>

namespace odRfl
{

    ObjectBuilderProbe::ObjectBuilderProbe()
    {
    }

    void ObjectBuilderProbe::readFieldRecord(od::DataReader &dr)
    {
        uint32_t fieldCount;
        uint32_t dwordCount;
        std::vector<char> fieldData;
        std::vector<uint16_t> fieldIndices;
        dr >> dwordCount  // note: the order of these two is reversed in respect to class records
           >> fieldCount;

        size_t dataFieldOffset = dr.tell();
        dr.ignore(dwordCount*4);

        fieldIndices.resize(fieldCount);
        for(size_t i = 0; i < fieldCount; ++i)
        {
            dr >> fieldIndices[i];
        }

        for(size_t i = 0; i < fieldCount; ++i)
        {
            uint32_t typeId;
            std::string name;
            dr >> typeId
               >> name;

            if(i >= fieldIndices.size() || fieldIndices[i] >= mFieldRegistrations.size())
            {
                // not an error. might be we just have loaded an object with unimplemented fields/RflClass
                continue;
            }

            FieldRegistration &reg = mFieldRegistrations[fieldIndices[i]];

            Field::Type fieldType = static_cast<Field::Type>(typeId & 0xff);
            bool isArray = (typeId & 0x1000) || (fieldType == Field::Type::STRING); // strings are stored exactly like arrays

            if(reg.field.getFieldType() != fieldType)
            {
                throw od::Exception("Field type mismatch in object record. Field type as defined in RflClass does not match the one found in record.");
            }

            if(reg.fieldName != name) // TODO: costly comparison. might want to make this optional
            {
                Logger::error() << "Field name mismatch: Field in RflClass was named '" << reg.fieldName << "' where field in object record was named '" << name << "'";
                throw od::Exception("Field name mismatch in object record. Field name as defined in RflClass does not match the one found in object record.");
            }

            if(reg.field.isArray() != isArray)
            {
                Logger::error() << "Field array flag mismatch: Field '" << name << "' was array in RFL or object record while in the other it was not.";
                throw od::Exception("Field as defined in RflClass does not match array state as found in object record.");
            }

            size_t fieldDefEndOffset = dr.tell();

            // everything seems to be in order. fill field

            dr.seek(dataFieldOffset + i*4);
            if(!isArray)
            {
                reg.field.fill(dr);

            }else
            {
                uint16_t offset;
                uint16_t length;
                dr >> offset
                   >> length;

                dr.seek(dataFieldOffset + offset*4);
                reg.field.fillArray(length, dr);
            }

            dr.seek(fieldDefEndOffset);

            Logger::debug() << "Overwrote field '" << name << "' from object record";
        }
    }

    void ObjectBuilderProbe::beginCategory(const char *categoryName)
    {
    }

    void ObjectBuilderProbe::registerField(Field &field, const char *fieldName)
    {
        mFieldRegistrations.push_back(FieldRegistration(field, fieldName));
    }

}
