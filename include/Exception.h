/*
 * Exception.h
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_EXCEPTION_H_
#define INCLUDE_EXCEPTION_H_

#include <stdexcept>

#include "SrscFile.h"

namespace od
{

	typedef std::runtime_error Exception;

	class NotFoundException : public Exception
	{
	public:

		NotFoundException(const char *msg, RecordType type = 0, RecordId id = 0);

		inline RecordType getRecordType() const { return mType; };
		inline RecordId getRecordId() const { return mId; };


	private:

		RecordType mType;
		RecordId mId;

	};

	class InvalidArgumentException : public Exception
	{
	public:

		InvalidArgumentException(const char *msg);

	};

	class UnsupportedException : public Exception
	{
	public:

		UnsupportedException(const char *msg);

	};

	class IoException : public Exception
	{
	public:

		IoException(const char *msg);

	};
}


#endif /* INCLUDE_EXCEPTION_H_ */
