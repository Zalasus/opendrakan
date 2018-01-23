/*
 * ZStream.cpp
 *
 *  Created on: 23 Jan 2018
 *      Author: zal
 */

#include "ZStream.h"

#include "Exception.h"

namespace od
{

    ZStreamBuffer::ZStreamBuffer(std::istream &in, size_t bufferSize)
    : mInputStream(in)
    , mInputBuffer(bufferSize)
    , mInputAvailable(0)
    , mOutputBuffer(bufferSize)
    , mOutputAvailable(0)
    , mOutputCurrent(0)
    {
        mZStream.zalloc = Z_NULL;
        mZStream.zfree = Z_NULL;
        mZStream.opaque = Z_NULL;
        mZStream.avail_in = 0;
        mZStream.next_in = Z_NULL;

        int ret = inflateInit(&mZStream);
        if(ret != Z_OK)
        {
            throw Exception("Could not initialize zlib");
        }
    }

    ZStreamBuffer::~ZStreamBuffer()
    {
        inflateEnd(&mZStream);
    }

    ZStreamBuffer::int_type ZStreamBuffer::underflow()
    {
        if(mOutputCurrent >= mOutputAvailable)
        {
            _fillBuffer();

            if(mOutputAvailable == 0)
            {
                return traits_type::eof();
            }
        }

        return traits_type::to_int_type(mOutputBuffer[mOutputCurrent]);
    }

    ZStreamBuffer::int_type ZStreamBuffer::uflow()
    {
        if(mOutputCurrent >= mOutputAvailable)
        {
            _fillBuffer();

            if(mOutputAvailable == 0)
            {
                return traits_type::eof();
            }
        }

        return traits_type::to_int_type(mOutputBuffer[mOutputCurrent++]);
    }

    ZStreamBuffer::int_type ZStreamBuffer::pbackfail(int_type ch)
    {
        return traits_type::eof();
    }

    std::streamsize ZStreamBuffer::showmanyc()
    {
        return mOutputAvailable - mOutputCurrent;
    }

    void ZStreamBuffer::_fillBuffer()
    {
        mZStream.avail_in = mInputStream.readsome(reinterpret_cast<char*>(mInputBuffer.ptr()), mInputBuffer.size());
        if(mZStream.avail_in == 0)
        {
            mOutputAvailable = 0;
            return;
        }
        mZStream.next_in = mInputBuffer.ptr();

        mZStream.avail_out = mOutputBuffer.size();
        mZStream.next_out = mOutputBuffer.ptr();

        int zlibError = inflate(&mZStream, Z_SYNC_FLUSH);
        switch(zlibError)
        {
        case Z_OK:
            break;

        case Z_STREAM_END:
            break; // What to do here?

        default:
            throw Exception("ZLib inflate error");
        }

        mOutputAvailable = mOutputBuffer.size() - mZStream.avail_out;
        mOutputCurrent = 0;
    }

}

