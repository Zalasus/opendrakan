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

		virtual ~RflFieldProbe() {};

		virtual void registerField(RflField &field, RflField::RflFieldType fieldType, const std::string &fieldName) = 0;
	};

	class RflClassBuilder : public RflFieldProbe
	{
	public:

		RflClassBuilder();

		// implement RflFieldProbe
		void registerField(RflField &field, RflField::RflFieldType fieldType, const std::string &fieldName) override;

		void readFieldRecord(DataReader &dr, bool isObjectRecord);
		void fillFields();


	private:

		struct FieldReg
		{
			RflField &field;
			RflField::RflFieldType fieldType;
	        std::string fieldName;
		};

		struct FieldEntry
		{
			RflField::RflFieldType fieldType;
			bool isArray;
			uint16_t index;
	        std::string fieldName;
		};

	    std::vector<FieldReg> mFieldRegs;
	    std::vector<FieldEntry> mFieldEntries;
	    std::vector<char> mFieldData;
	};

}

#endif /* INCLUDE_RFL_RFLCLASSBUILDER_H_ */
