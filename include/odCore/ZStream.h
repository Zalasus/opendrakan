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

        inline std::streamoff getZlibDataStart() { return mZlibDataStart; };
        inline std::streamoff getZlibDataEnd() { return mZlibDataEnd; };

        void seekToEndOfZlib();

        /**
         * Will restart the zstream so it can be used to read another compressed block without having to reload buffers etc.
         * Will do nothing if stream has not yet ended.
         */
        void restart();


    protected:

        virtual int_type underflow() override;


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
        bool mStreamEnded;
        std::streamoff mZlibDataStart;
        std::streamoff mZlibDataEnd;
    };

    class ZStream : public std::istream
    {
    public:

        ZStream(std::istream &in);
        ~ZStream();

        inline std::streamoff getZlibDataStart() { return mBuffer->getZlibDataStart(); };
        inline std::streamoff getZlibDataEnd() { return mBuffer->getZlibDataEnd(); };
        inline void seekToEndOfZlib() { mBuffer->seekToEndOfZlib(); };

    private:

        ZStreamBuffer *mBuffer;
    };

}

#endif /* INCLUDE_ZSTREAM_H_ */
