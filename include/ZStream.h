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
#include <vector>
#include <zlib.h> // has C-linkage built in

#define OD_ZSTREAM_DEFAULT_BUFFER_SIZE (1 << 19)

namespace od
{

    class ZStreamBuffer : public std::streambuf
    {
    public:

        ZStreamBuffer(std::istream &in, size_t bufferSize = OD_ZSTREAM_DEFAULT_BUFFER_SIZE);
        ~ZStreamBuffer();

        /**
         * Will move the input stream just past the last byte that has been used for generating output, or
         * to the beginning of the zlib data block if the zlib stream hasn't ended yet.
         * It will then clear all internal buffers.
         */
        void finalize();

    protected:

        virtual int_type underflow();


    private:

        void _error(int zlibError);


        std::istream &mInputStream;

        std::vector<Bytef> mInputBuffer;
        Bytef *mInputStart;
        Bytef *mInputEnd;

        std::vector<Bytef> mOutputBuffer;
        Bytef *mOutputEnd;

        z_stream mZStream;

        bool mStreamActive;
        std::streamoff mZlibEndInInput;

    };

    class ZStream : public std::istream
    {
    public:

        ZStream(std::istream &in);
        ~ZStream();

        void finalize();

    private:

        ZStreamBuffer *mBuffer;
    };

}

#endif /* INCLUDE_ZSTREAM_H_ */
