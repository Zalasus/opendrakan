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

#define OD_ZSTREAM_DEFAULT_BUFFER_SIZE (1 << 19)

namespace od
{

    class ZStreamBuffer : public std::streambuf
    {
    public:

        ZStreamBuffer(std::istream &in, size_t bufferSize = OD_ZSTREAM_DEFAULT_BUFFER_SIZE);
        ~ZStreamBuffer();


    protected:

        virtual int_type underflow();


    private:

        void _fillBuffer();


        std::istream &mInputStream;

        SimpleArray<Bytef> mInputBuffer;
        Bytef *mInputStart;
        Bytef *mInputEnd;

        SimpleArray<Bytef> mOutputBuffer;
        Bytef *mOutputEnd;

        z_stream mZStream;

    };

    class ZStream : public std::istream
    {
    public:

        ZStream(std::istream &in);
        ~ZStream();

    };

}

#endif /* INCLUDE_ZSTREAM_H_ */
