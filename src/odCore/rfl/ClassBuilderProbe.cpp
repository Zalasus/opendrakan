/*
 * ClassBuilderProbe.cpp
 *
 *  Created on: Sep 17, 2018
 *      Author: zal
 */

#include <odCore/rfl/ClassBuilderProbe.h>

#include <algorithm>

#include <odCore/Exception.h>
#include <odCore/rfl/Field.h>

namespace odRfl
{

    ClassBuilderProbe::ClassBuilderProbe()
    : mRegistrationIndex(0)
    {
    }

    void ClassBuilderProbe::readFieldRecord(od::DataReader &dr)
    {
        uint32_t fieldCount;
        uint32_t dwordCount;
        std::vector<char> fieldData;
        std::vector<uint16_t> fieldIndices;
        dr >> fieldCount
           >> dwordCount;

        mFieldData.resize(dwordCount*4);
        dr.read(mFieldData.data(), mFieldData.size());

        mFieldEntries.resize(fieldCount);
        for(size_t i = 0; i < fieldCount; ++i)
        {
            uint32_t type;
            std::string name;
            dr >> type
               >> name;

            mFieldEntries[i].fieldType = type & 0xff;
            mFieldEntries[i].fieldName = name;
            mFieldEntries[i].index = i;
            mFieldEntries[i].dataOffset = i*4;
            mFieldEntries[i].isArray = (type & 0x1000) || (mFieldEntries[i].fieldType == static_cast<uint32_t>(Field::Type::STRING)); // strings are stored exactly like arrays
        }
    }

    void ClassBuilderProbe::resetIndexCounter()
    {
        mRegistrationIndex = 0;
    }

    void ClassBuilderProbe::beginCategory(const char *categoryName)
    {
        // Class builder doesn't give a damn about categories
    }

    void ClassBuilderProbe::registerField(Field &field, const char *fieldName)
    {
        if(mRegistrationIndex >= mFieldEntries.size())
        {
            throw od::Exception("More fields probed than found in class record. Did you call resetIndexCounter() before probing?");
        }

        FieldEntry &entry = mFieldEntries[mRegistrationIndex];

        if(entry.fieldType != static_cast<uint32_t>(field.getFieldType()))
        {
            throw od::Exception("Type mismatch in class record. Field type as defined in RflClass does not match the one found in record.");
        }

        if(entry.fieldName != fieldName) // TODO: costly comparison. might want to make this optional
        {
            Logger::error() << "Field name mismatch: Field in RflClass was named '" << fieldName << "' where field in record was named '" << entry.fieldName << "'";
            throw od::Exception("Field name mismatch in class record. Field name as defined in RflClass does not match the one found in record.");
        }

        if(entry.isArray != field.isArray())
        {
            Logger::error() << "Field array flag mismatch: Field '" << fieldName << "' was array in RFL or file while in the other it was not.";
            throw od::Exception("Field as defined in RflClass does not match array state as found in class record.");
        }

        // field seems reasonable. let's fill it

        od::MemBuffer buf(mFieldData.data(), mFieldData.data() + mFieldData.size());
        std::istream dataStr(&buf);
        od::DataReader dr(dataStr);

        dr.seek(entry.dataOffset);
        if(!entry.isArray)
        {
            field.fill(dr);

        }else
        {
            uint16_t offset;
            uint16_t length;
            dr >> offset
               >> length;

            dr.seek(offset*4);
            field.fillArray(length, dr);
        }

        Logger::debug() << "Filled field '" << fieldName << "'";

        ++mRegistrationIndex;
    }

}

