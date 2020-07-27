/*
 * Exception.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include <odCore/Exception.h>

#include <sstream>

namespace od
{
    Exception::Exception()
	{
	}

	Exception::Exception(const std::string &msg)
	: mMsg(msg)
	{
	}

	const char *Exception::what() const noexcept
	{
		return mMsg.c_str();
	}


	NotFoundException::NotFoundException(const std::string &msg)
	: Exception(msg)
	{
	}


	InvalidArgumentException::InvalidArgumentException(const std::string &msg)
	: Exception(msg)
	{
	}


	UnsupportedException::UnsupportedException(const std::string &msg)
	: Exception(msg)
	{
	}


	IoException::IoException(const std::string &msg)
	: Exception(msg)
	{
	}


    UnreachableException::UnreachableException(const char *file, const char *function, int line)
    {
        std::ostringstream oss;
        oss << "Reached supposedly unreachable code in function " << function
            << " in file " << file << " @ line " << line;
        mMsg = oss.str();
    }


    UnimplementedException::UnimplementedException(const char *file, const char *function, int line)
    {
        std::ostringstream oss;
        oss << "Function " << function
            << " in file " << file << " @ line " << line
            << " is unimplemented";
        mMsg = oss.str();
    }

}
