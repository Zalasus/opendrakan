/*
 * Exception.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "Exception.h"

namespace od
{

	NotFoundException::NotFoundException(const char *msg, RecordType type, RecordId id)
	: Exception(msg)
	, mType(type)
	, mId(id)
	{
	}


	InvalidArgumentException::InvalidArgumentException(const char *msg)
	: Exception(msg)
	{
	}

	UnsupportedException::UnsupportedException(const char *msg)
	: Exception(msg)
	{
	}

	IoException::IoException(const char *msg)
	: Exception(msg)
	{
	}

}
