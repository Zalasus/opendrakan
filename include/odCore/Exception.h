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

        Exception();
		Exception(const std::string &msg);

		virtual const char *what() const noexcept;


	protected:

		std::string mMsg;
	};

	class NotFoundException : public Exception
	{
	public:

		NotFoundException(const std::string &msg);

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

    class UreachableException: public Exception
	{
    public:

        UreachableException(const char *file, const char *function, int line);

    };

    class UnimplementedException : public Exception
	{
    public:

        UnimplementedException(const char *file, const char *function, int line);

    };
}

#define OD_UNREACHABLE() throw od::UnreachableException(__FILE__, __func__, __LINE__)
#define OD_UNIMPLEMENTED() throw od::UnimplementedException(__FILE__, __func__, __LINE__)
#define OD_CHECK_ARG_NONNULL(arg) if((arg) == nullptr) throw od::IllegalArgumentException("Argument " #arg " was nullptr")


#endif /* INCLUDE_EXCEPTION_H_ */
