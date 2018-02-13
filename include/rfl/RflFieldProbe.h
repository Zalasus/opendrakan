/*
 * RflFieldProbe.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLCLASSBUILDER_H_
#define INCLUDE_RFL_RFLCLASSBUILDER_H_

#include <string>
#include <vector>

#include "DataStream.h"
#include "RflField.h"
#include "Exception.h"

namespace od
{

	class RflFieldProbe
	{
	public:

		friend class RflField;
		friend class RflClassBuilder;

	private:

		struct FieldReg
		{
			RflField &field;
			RflField::RflFieldType fieldType;
	        std::string fieldName;
		};

		// this method is only to be used by RflField
		void registerField(RflField &field, RflField::RflFieldType fieldType, const std::string &fieldName);

		// these methods are for RflClassBuilder
		FieldReg &getFieldReg(size_t i);

		std::vector<FieldReg> mFieldRegs;
	};

	class RflClassBuilder
	{
	public:

		void readFieldRecord(DataReader &dr, bool isObjectRecord);

		void fillFields(RflFieldProbe &probe);


	private:

		struct FieldEntry
		{
			RflField::RflFieldType fieldType;
			bool isArray;
			uint16_t index;
			size_t dataOffset; // offset of entry in mFieldData
	        std::string fieldName;
		};

	    std::vector<FieldEntry> mFieldEntries;
	    std::vector<char> mFieldData;
	};


}

#endif /* INCLUDE_RFL_RFLCLASSBUILDER_H_ */
