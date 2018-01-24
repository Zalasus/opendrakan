/*
 * ZStream.cpp
 *
 *  Created on: 23 Jan 2018
 *      Author: zal
 */

#include "ZStream.h"

#include "Logger.h"
#include "Exception.h"

namespace od
{

    ZStreamBuffer::ZStreamBuffer(std::istream &in, size_t bufferSize)
    : mInputStream(in)
    , mInputBuffer(bufferSize)
    , mInputStart(nullptr)
    , mInputEnd(nullptr)
    , mOutputBuffer(bufferSize)
    , mOutputEnd(nullptr)
    {
        mZStream.zalloc = Z_NULL;
        mZStream.zfree = Z_NULL;
        mZStream.opaque = Z_NULL;
        mZStream.avail_in = 0;
        mZStream.next_in = Z_NULL;

        int ret = inflateInit(&mZStream);
        if(ret != Z_OK)
        {
        	_error(ret);
        }

        char *gptr = reinterpret_cast<char*>(mOutputBuffer.ptr());

        this->setg(gptr, gptr, gptr);
    }

    ZStreamBuffer::~ZStreamBuffer()
    {
        inflateEnd(&mZStream);
    }

    ZStreamBuffer::int_type ZStreamBuffer::underflow()
    {
        // fill input buffer if no input available anymore
        if(mInputStart == mInputEnd)
        {
            mInputStart = mInputBuffer.ptr();
            mInputStream.read(reinterpret_cast<char*>(mInputBuffer.ptr()), mInputBuffer.size());
            mInputEnd = mInputBuffer.ptr() + mInputStream.gcount();

            if(mInputEnd == mInputStart)
            {
                // no input was read. can't produce more output -> EOF
                return traits_type::eof();
            }
        }

        Logger::info() << "Call to inflate";

        Bytef a = *mInputStart;
        Bytef b = *(mInputStart+1);

        Logger::info() << "Header: " << (int)a << " " << (int)b;

        // decompress
        mZStream.next_in = mInputStart;
        mZStream.avail_in = mInputEnd - mInputStart;
        mZStream.next_out = mOutputBuffer.ptr();
        mZStream.avail_out = mOutputBuffer.size();
        int ret = inflate(&mZStream, Z_NO_FLUSH);
        if(ret != Z_OK && ret != Z_STREAM_END)
        {
            _error(ret);
        }

        // update our pointers and set gptr
        mInputStart = mZStream.next_in;
        mInputEnd = mInputBuffer.ptr() + mZStream.avail_in;
        mOutputEnd = mZStream.next_out;

        char *outputStart = reinterpret_cast<char*>(mOutputBuffer.ptr());
        char *outputEnd = reinterpret_cast<char*>(mOutputEnd);
        setg(outputStart, outputStart, outputEnd);

        return (gptr() == egptr()) ? traits_type::eof() : traits_type::to_int_type(*gptr());
    }

    void ZStreamBuffer::_error(int zlibError)
    {
    	std::string msg = mZStream.msg;

    	msg += " (";

    	switch(zlibError)
        {
        case Z_STREAM_ERROR:
            msg += "Z_STREAM_ERROR";
            break;

        case Z_DATA_ERROR:
            msg += "Z_DATA_ERROR";
            break;

        case Z_MEM_ERROR:
            msg += "Z_MEM_ERROR";
            break;

        case Z_VERSION_ERROR:
            msg += "Z_VERSION_ERROR";
            break;

        case Z_BUF_ERROR:
            msg += "Z_BUF_ERROR";
            break;

        default:
            std::ostringstream oss;
            oss << zlibError;
            msg += oss.str();
            break;
        }

    	msg += ")";

        throw Exception(msg);
    }



    ZStream::ZStream(std::istream &in)
    : std::istream(new ZStreamBuffer(in))
    {
    	exceptions(std::ios_base::badbit);
    }

    ZStream::~ZStream()
    {
        delete rdbuf();
    }

}

