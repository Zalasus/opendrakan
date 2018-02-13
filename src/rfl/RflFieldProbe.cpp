/*
 * RflFieldProbe.cpp
 *
 *  Created on: 31 Jan 2018
 *      Author: zal
 */

#include "rfl/RflFieldProbe.h"

namespace od
{

	void RflFieldProbe::registerField(RflField &field, RflField::RflFieldType fieldType, const std::string &fieldName)
	{
		FieldReg f =
		{
			.field = field,
			.fieldType = fieldType,
			.fieldName = fieldName
		};

		mFieldRegs.push_back(f);
	}

	RflFieldProbe::FieldReg &RflFieldProbe::getFieldReg(size_t i)
	{
		if(i >= mFieldRegs.size())
		{
			throw Exception("Index of requested field out of bounds");
		}

		return mFieldRegs[i];
	}



    void RflClassBuilder::readFieldRecord(DataReader &dr, bool isObjectRecord)
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

    		mFieldEntries[i].fieldType = static_cast<RflField::RflFieldType>(type & 0xff);
    		mFieldEntries[i].fieldName = name;
    		mFieldEntries[i].index = isObjectRecord ? fieldIndices[i] : i;
    		mFieldEntries[i].dataOffset = i*4;
    		mFieldEntries[i].isArray = (type & 0x1000) || (mFieldEntries[i].fieldType == RflField::STRING); // strings are stored exactly like arrays
    	}
    }

    void RflClassBuilder::fillFields(RflFieldProbe &probe)
    {
    	MemBuffer buf(mFieldData.data(), mFieldData.data() + mFieldData.size());
    	std::istream dataStr(&buf);
    	DataReader dr(dataStr);

    	for(auto it = mFieldEntries.begin(); it != mFieldEntries.end(); ++it)
    	{
    		RflFieldProbe::FieldReg &reg = probe.getFieldReg(it->index);

    		if(it->fieldType != reg.fieldType)
    		{
    			throw Exception("Type mismatch in RflClass. Field type as defined in RflClass does not match the one found in record.");
    		}

    		if(it->fieldName != reg.fieldName)
    		{
    			Logger::error() << "Field name mismatch: Field in RflClass was named '" << reg.fieldName << "' where field in record was named '" << it->fieldName << "'";
    			throw Exception("Field name mismatch in RflClass. Field name as defined in RflClass does not match the one found in record.");
    		}

    		if(it->isArray != reg.field.isArray())
    		{
    			throw Exception("Field as defined in RflClass does not match array state as found in record.");
    		}

    		// field seems reasonable. let's fill it

    		dr.seek(it->dataOffset);
    		if(!it->isArray)
    		{
    			reg.field.fill(dr);

    		}else
    		{
    			uint16_t offset;
    			uint16_t length;
    			dr >> offset
				   >> length;

    			dr.seek(offset*4);
    			reg.field.fillArray(length, dr);
    		}

    		Logger::debug() << "Filled field '" << reg.fieldName << "'";
    	}
    }
}


