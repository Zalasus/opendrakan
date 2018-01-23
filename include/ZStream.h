/*
 * ZStream.h
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#ifndef INCLUDE_ZSTREAM_H_
#define INCLUDE_ZSTREAM_H_

#include <istream>
#include <streambuf>
#include <zlib.h> // has C-linkage built in

#include "SimpleArray.h"

namespace od
{

    class ZStreamBuffer : std::streambuf
    {
    public:

        ZStreamBuffer(std::istream &in, size_t bufferSize);
        ~ZStreamBuffer();


    private:

        virtual int_type underflow();
        virtual int_type uflow();
        virtual int_type pbackfail(int_type ch);
        virtual std::streamsize showmanyc();


    private:

        void _fillBuffer();


        std::istream &mInputStream;

        SimpleArray<Bytef> mInputBuffer;
        size_t mInputAvailable;

        SimpleArray<Bytef> mOutputBuffer;
        size_t mOutputAvailable;
        size_t mOutputCurrent;

        z_stream mZStream;

    };

}

#endif /* INCLUDE_ZSTREAM_H_ */
