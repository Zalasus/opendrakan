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

	    struct RflFieldReg
	    {
	        RflField &field;
	        RflField::RflFieldType fieldType;
	        std::string fieldName;
	    };

	    void registerField(RflField &field, RflField::RflFieldType fieldType, const std::string &fieldName);

	    template <typename T>
	    T &getFieldAs(uint32_t index);


	private:

	    std::vector<RflFieldReg> mFields;
	};


	template <typename T>
	T &RflFieldProbe::getFieldAs(uint32_t index)
	{
	    if(index >= mFields.size())
	    {
	        throw Exception("No such field");
	    }

	    RflFieldReg &reg = mFields[index];

	    switch(reg.fieldType)
	    {
	    case RflField::INTEGER:
	    case RflField::FLOAT:
	    case RflField::CLASS:
	    case RflField::MODEL:
	    case RflField::SOUND:
	    case RflField::ENUM:
	    case RflField::CHAR_CHANNEL:
	    case RflField::ANIMATION:
	    case RflField::STRING:
	    case RflField::SEUQUENCE:
	    case RflField::TEXTURE:
	    case RflField::COLOR:
	    default:
	        throw Exception("Unknown field type");
	    }
	}

}

#endif /* INCLUDE_RFL_RFLCLASSBUILDER_H_ */
