/*
 * RflFieldProbe.cpp
 *
 *  Created on: 31 Jan 2018
 *      Author: zal
 */

#include "rfl/RflFieldProbe.h"

#include <algorithm>

#include "Exception.h"
#include "rfl/RflField.h"

namespace odRfl
{

    void RflFieldProbe::beginCategory(const char *categoryName)
    {
    }

    void RflFieldProbe::registerField(RflField &field, const char *fieldName)
    {
    }

    void RflFieldProbe::registerField(RflAssetRef &field, const char *fieldName)
    {
        this->registerField((RflField&)field, fieldName);
    }


    RflClassBuilder::RflClassBuilder()
    : mRegistrationIndex(0)
    {
    }

    void RflClassBuilder::readFieldRecord(od::DataReader &dr, bool isObjectRecord)
    {
    	uint32_t fieldCount;
    	uint32_t dwordCount;
    	std::vector<char> fieldData;
    	std::vector<uint16_t> fieldIndices;
    	dr >> fieldCount
		   >> dwordCount;

    	if(isObjectRecord)
    	{
    		std::swap(fieldCount, dwordCount);
    	}

    	mFieldData.resize(dwordCount*4);
    	dr.read(mFieldData.data(), mFieldData.size());

    	if(isObjectRecord)
    	{
    		fieldIndices.resize(fieldCount);
    		for(size_t i = 0; i < fieldCount; ++i)
    		{
    			dr >> fieldIndices[i];
    		}
    	}

    	mFieldEntries.resize(fieldCount);
    	for(size_t i = 0; i < fieldCount; ++i)
    	{
    		uint32_t type;
    		std::string name;
    		dr >> type
			   >> name;

    		mFieldEntries[i].fieldType = type & 0xff;
    		mFieldEntries[i].fieldName = name;
    		mFieldEntries[i].index = isObjectRecord ? fieldIndices[i] : i;
    		mFieldEntries[i].dataOffset = i*4;
    		mFieldEntries[i].isArray = (type & 0x1000) || (mFieldEntries[i].fieldType == RflField::STRING); // strings are stored exactly like arrays
    	}
    }

    void RflClassBuilder::resetIndexCounter()
    {
    	mRegistrationIndex = 0;
    }

    void RflClassBuilder::beginCategory(const char *categoryName)
    {
        // Class builder doesn't give a damn about categories
    }

    void RflClassBuilder::registerField(RflField &field, const char *fieldName)
    {
        auto pred = [this](FieldEntry &e){ return e.index == mRegistrationIndex; };
        auto it = std::find_if(mFieldEntries.begin(), mFieldEntries.end(), pred); // FIXME: O(n) lookup. might split into classbuilder and objectbuilder, one with map and one with array
        if(it == mFieldEntries.end())
        {
            mRegistrationIndex++;
            return; // FIXME: this should be an error when building a class
        }

        if(it->fieldType != field.getFieldType())
        {
            throw od::Exception("Type mismatch in RflClass. Field type as defined in RflClass does not match the one found in record.");
        }

        if(it->fieldName != fieldName) // TODO: costly comparison. might want to make this optional
        {
            Logger::error() << "Field name mismatch: Field in RflClass was named '" << fieldName << "' where field in record was named '" << it->fieldName << "'";
            throw od::Exception("Field name mismatch in RflClass. Field name as defined in RflClass does not match the one found in record.");
        }

        if(it->isArray != field.isArray())
        {
            Logger::error() << "Field array flag mismatch: Field '" << fieldName << "' was array in RFL or file while in the other it was not.";
            throw od::Exception("Field as defined in RflClass does not match array state as found in record.");
        }

        // field seems reasonable. let's fill it

        od::MemBuffer buf(mFieldData.data(), mFieldData.data() + mFieldData.size());
        std::istream dataStr(&buf);
        od::DataReader dr(dataStr);

        dr.seek(it->dataOffset);
        if(!it->isArray)
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


