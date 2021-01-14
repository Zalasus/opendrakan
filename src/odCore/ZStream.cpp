/*
 * ZStream.cpp
 *
 *  Created on: 23 Jan 2018
 *      Author: zal
 */

#include <odCore/ZStream.h>

#include <odCore/Panic.h>

namespace od
{

    const size_t ZStreamBuffer::DefaultBufferSize = (1 << 19);

    ZStreamBuffer::ZStreamBuffer(std::istream &in, size_t inputBufferSize, size_t outputBufferSize)
    : mInputStream(in)
    , mInputBuffer(inputBufferSize)
    , mInputStart(nullptr)
    , mInputEnd(nullptr)
    , mOutputBuffer(outputBufferSize)
    , mOutputEnd(nullptr)
    , mStreamActive(false)
    , mStreamEnded(false)
    , mZlibDataStart(in.tellg())
    , mZlibDataEnd(in.tellg())
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
    		mStreamActive = false;
    	}
    }

    void ZStreamBuffer::seekToEndOfZlib()
    {
    	if(!mStreamEnded)
    	{
    		OD_PANIC() << "Can't seek to end of zlib data. Stream has not ended yet";
    	}

    	mInputStream.clear(); // as buffer reading had us hitting EOF most likely
    	mInputStream.seekg(getZlibDataEnd());
    }

    void ZStreamBuffer::restart()
    {
    	mStreamEnded = false;
    }

    ZStreamBuffer::int_type ZStreamBuffer::underflow()
    {
    	if(mStreamEnded)
    	{
    		return traits_type::eof();
    	}

    	if(!mStreamActive && !mStreamEnded)
    	{
    		//Logger::debug() << "Zstream not yet active. Activating";

    		mZlibDataStart = mInputStream.tellg();

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

        // decompress
        mOutputEnd = mOutputBuffer.data();
        do
        {
        	// fill input buffer if no input available anymore
			if(mInputStart == mInputEnd)
			{
				//Logger::debug() << "Filling input buffer";
				mInputStart = mInputBuffer.data();
				mInputStream.read(reinterpret_cast<char*>(mInputBuffer.data()), mInputBuffer.size());
				mInputEnd = mInputBuffer.data() + mInputStream.gcount();

				if(mInputEnd == mInputStart)
				{
					// no input was read. can't produce more output -> EOF
					return traits_type::eof();
				}
			}

			/*if(initialRun)
			{
				Logger::debug() << "First call to inflate. Header: " << std::hex << (int)(*mInputStart) << " " << (int)(*mInputStart+1) << std::dec;

			}else
			{
				Logger::debug() << "Repeated call to inflate";
			}*/

			mZStream.next_in = mInputStart;
			mZStream.avail_in = mInputEnd - mInputStart;
			mZStream.next_out = mOutputBuffer.data();
			mZStream.avail_out = mOutputBuffer.size();
			int ret = inflate(&mZStream, Z_NO_FLUSH);
			if(ret != Z_OK && ret != Z_STREAM_END)
			{
				_error(ret);
			}

			mInputStart = mZStream.next_in;
			mInputEnd = mInputStart + mZStream.avail_in;
			mOutputEnd = mZStream.next_out;

			if(ret == Z_STREAM_END)
			{
				// all zlib data has been decompressed. avail_in contains the amount of bytes
				//  read from stream that we didn't use

				//Logger::debug() << "Zstream has ended";

				mInputStream.clear(); // important!! tellg tells rubbish otherwise
				mZlibDataEnd = (int)mInputStream.tellg() - mZStream.avail_in;
				//Logger::debug() << "We have " << mZStream.avail_in << " bytes left in input buffer. ZlibEnd is at " << mZlibDataEnd;
				mStreamActive = false;
				mStreamEnded = true;
				inflateEnd(&mZStream);
				break;
			}

			// keep looping while until inflate() produced output or tells us the stream has ended
        } while(mOutputEnd == mOutputBuffer.data());

        char *outputStart = reinterpret_cast<char*>(mOutputBuffer.data());
        char *outputEnd = reinterpret_cast<char*>(mOutputEnd);
        setg(outputStart, outputStart, outputEnd);

        return (outputStart == outputEnd) ? traits_type::eof() : traits_type::to_int_type(*gptr());
    }

    void ZStreamBuffer::_error(int zlibError)
    {
        const char *msg = "";
    	switch(zlibError)
        {
        case Z_STREAM_ERROR:
            msg = "Z_STREAM_ERROR";
            break;

        case Z_DATA_ERROR:
            msg = "Z_DATA_ERROR";
            break;

        case Z_MEM_ERROR:
            msg = "Z_MEM_ERROR";
            break;

        case Z_VERSION_ERROR:
            msg = "Z_VERSION_ERROR";
            break;

        case Z_BUF_ERROR:
            msg = "Z_BUF_ERROR";
            break;

        default:
            msg = "unknown zlib error";
            break;
        }

        OD_PANIC() << mZStream.msg << " (" << msg << ")";
    }



    ZStream::ZStream(std::istream &in)
    : std::istream(new ZStreamBuffer(in))
    {
    	exceptions(std::ios_base::badbit);

    	mBuffer = static_cast<ZStreamBuffer*>(rdbuf());
    }

    ZStream::ZStream(std::istream &in, size_t inputBufferSize, size_t outputBufferSize)
    : std::istream(new ZStreamBuffer(in, inputBufferSize, outputBufferSize))
    {
        exceptions(std::ios_base::badbit);

        mBuffer = static_cast<ZStreamBuffer*>(rdbuf());
    }

    ZStream::~ZStream()
    {
    	delete rdbuf();
    }

}
