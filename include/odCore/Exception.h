/*
 * Exception.h
 *
 *  Created on: 9 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_EXCEPTION_H_
#define INCLUDE_EXCEPTION_H_

#include <exception>
#include <string>

#include <odCore/SrscFile.h>

namespace od
{

	class Exception : public std::exception
	{
	public:

		Exception(const std::string &msg);

		virtual const char *what() const noexcept;


	protected:

		std::string mMsg;
	};

	class NotFoundException : public Exception
	{
	public:

		NotFoundException(const std::string &msg, RecordType type = 0, RecordId id = 0);

		inline RecordType getRecordType() const { return mType; };
		inline RecordId getRecordId() const { return mId; };


	private:

		RecordType mType;
		RecordId mId;

	};

	class InvalidArgumentException : public Exception
	{
	public:

		InvalidArgumentException(const std::string &msg);

	};

	class UnsupportedException : public Exception
	{
	public:

		UnsupportedException(const std::string &msg);

	};

	class IoException : public Exception
	{
	public:

		IoException(const std::string &msg);

	};
}


#endif /* INCLUDE_EXCEPTION_H_ */
