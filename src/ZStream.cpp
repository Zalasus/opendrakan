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
    , mStreamActive(false)
    , mZlibEndInInput(0)
    {
    	mZStream.zalloc = Z_NULL;
        mZStream.zfree = Z_NULL;
        mZStream.opaque = Z_NULL;

        char *gptr = reinterpret_cast<char*>(mOutputBuffer.data());
        setg(gptr, gptr, gptr);
    }

    ZStreamBuffer::~ZStreamBuffer()
    {
    	if(mStreamActive)
    	{
    		inflateEnd(&mZStream);
    	}
    }

    void ZStreamBuffer::finalize()
    {
    	mInputStream.seekg(mZlibEndInInput);

    	mInputStart = mInputBuffer.data();
    	mInputEnd = mInputBuffer.data();
    	mOutputEnd = mOutputBuffer.data();

    	char *gptr = reinterpret_cast<char*>(mOutputBuffer.data());
        setg(gptr, gptr, gptr);
    }

    ZStreamBuffer::int_type ZStreamBuffer::underflow()
    {
    	if(!mStreamActive)
    	{
    		mZlibEndInInput = mInputStream.tellg();

    		mZStream.zalloc = Z_NULL;
    		mZStream.zfree = Z_NULL;
    		mZStream.opaque = Z_NULL;

    		int ret = inflateInit(&mZStream);
			if(ret != Z_OK)
			{
				_error(ret);
			}

			mStreamActive = true;
    	}

        // fill input buffer if no input available anymore
        if(mInputStart == mInputEnd)
        {
            mInputStart = mInputBuffer.data();
            mInputStream.read(reinterpret_cast<char*>(mInputBuffer.data()), mInputBuffer.size());
            mInputEnd = mInputBuffer.data() + mInputStream.gcount();

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
        mZStream.next_out = mOutputBuffer.data();
        mZStream.avail_out = mOutputBuffer.size();
        int ret = inflate(&mZStream, Z_NO_FLUSH);
        if(ret == Z_STREAM_END)
        {
        	// all zlib data has been decompressed. avail_in contains the amount of bytes
        	//  read from stream that we didn't use

        	mZlibEndInInput = (int)mInputStream.tellg() - mZStream.avail_in;
        	mStreamActive = false;
        	inflateEnd(&mZStream);

        } else if(ret != Z_OK)
        {
            _error(ret);
        }

        // update our pointers and set gptr
        mInputStart = mZStream.next_in;
        mInputEnd = mInputStart + mZStream.avail_in;
        mOutputEnd = mZStream.next_out;

        char *outputStart = reinterpret_cast<char*>(mOutputBuffer.data());
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

    	mBuffer = static_cast<ZStreamBuffer*>(rdbuf());
    }

    ZStream::~ZStream()
    {
    	delete rdbuf();
    }

    void ZStream::finalize()
    {
    	mBuffer->finalize();
    }

}

