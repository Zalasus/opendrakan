
#include <odCore/rfl/FieldLoaderProbe.h>

#include <algorithm>

#include <odCore/Panic.h>

#include <odCore/rfl/Field.h>

namespace odRfl
{

    FieldLoaderProbe::FieldLoaderProbe()
    : mRegistrationIndex(0)
    {
    }

    void FieldLoaderProbe::loadFromRecord(od::DataReader &dr, RecordFormat format)
    {
        uint32_t fieldCount;
        uint32_t dwordCount;
        switch(format)
        {
        case RecordFormat::CLASS:
            dr >> fieldCount
               >> dwordCount;
            break;

        case RecordFormat::OBJECT:
            dr >> dwordCount
               >> fieldCount;
            break;
        }

        mFieldEntries.resize(fieldCount);
        mFieldData.resize(dwordCount*4);
        dr.read(mFieldData.data(), mFieldData.size());

        if(format == RecordFormat::OBJECT)
        {
            for(size_t i = 0; i < fieldCount; ++i)
            {
                uint16_t index;
                dr >> index;

                mFieldEntries[i].index = index;
            }
        }

        for(size_t i = 0; i < fieldCount; ++i)
        {
            uint32_t type;
            std::string name;
            dr >> type
               >> name;

            if(format == RecordFormat::CLASS)
            {
                mFieldEntries[i].index = i;
            }

            mFieldEntries[i].fieldType = type & 0xff;
            mFieldEntries[i].fieldName = std::move(name);
            mFieldEntries[i].dataOffset = i*4;
            mFieldEntries[i].isArray = (type & 0x1000) || (mFieldEntries[i].fieldType == static_cast<uint32_t>(Field::Type::STRING)); // strings are stored exactly like arrays
        }

        // make sure the entry array is sorted by index so we can use a binary search by index later (since object format records can be sparse)
        auto pred = [](auto &left, auto &right){ return left.index < right.index; };
        std::sort(mFieldEntries.begin(), mFieldEntries.end(), pred);
    }

    void FieldLoaderProbe::reset()
    {
        mRegistrationIndex = 0;
    }

    void FieldLoaderProbe::beginCategory(const char *categoryName)
    {
    }

    void FieldLoaderProbe::registerField(Field &field, const char *fieldName)
    {
        auto currentFieldIndex = mRegistrationIndex++;

        auto pred = [](const FieldEntry &field, size_t index ){ return field.index < index; };
        auto it = std::lower_bound(mFieldEntries.begin(), mFieldEntries.end(), currentFieldIndex, pred);
        if(it == mFieldEntries.end() || it->index != currentFieldIndex)
        {
            // the field we are looking for does not exist in this record. for object records, we can ignore this. for class records, this would be an error TODO: report it
            return;
        }

        FieldEntry &entry = *it;

        if(entry.fieldType != static_cast<uint32_t>(field.getFieldType()))
        {
            OD_PANIC() << "Type mismatch in class record. Field type as defined in RflClass does not match the one found in record.";
        }

        if(entry.fieldName != fieldName) // TODO: costly comparison. might want to make this optional
        {
            OD_PANIC() << "Field name mismatch: Field in RflClass was named '" << fieldName << "' where field in record was named '" << entry.fieldName << "'";
        }

        if(entry.isArray != field.isArray())
        {
            OD_PANIC() << "Field array flag mismatch: Field '" << fieldName << "' was array in RFL or file while in the other it was not.";
        }

        // field seems reasonable. let's fill it

        od::MemoryInputBuffer buf(mFieldData.data(), mFieldData.size());
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
    }

}
