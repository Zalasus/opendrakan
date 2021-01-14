/*
 * RiffReader.cpp
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#include <odCore/RiffReader.h>

#include <cassert>

#include <odCore/Panic.h>

namespace od
{

    FourCC::FourCC()
    : code(0)
    {
    }

    FourCC::FourCC(uint32_t i)
    : FourCC(i, i >> 8, i >> 16, i >> 24)
    {
    }

    FourCC::FourCC(const char *c)
    : code(0)
    {
        if(c[0] == '\0' || c[1] == '\0' || c[2] == '\0' || c[3] == '\0')
        {
            OD_PANIC() << "FourCC string too short";
        }

        code = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
    }

    FourCC::FourCC(char c0, char c1, char c2, char c3)
    : code((c0 << 24) | (c1 << 16) | (c2 << 8) | c3)
    {
    }

    std::ostream &operator<<(std::ostream &lhs, const FourCC &rhs)
    {
        lhs << (char)(rhs.code >> 24)
            << (char)(rhs.code >> 16)
            << (char)(rhs.code >> 8)
            << (char)rhs.code;
        return lhs;
    }


    const FourCC RiffReader::RIFF_CHUNK_ID = FourCC("RIFF");
    const FourCC RiffReader::LIST_CHUNK_ID = FourCC("LIST");

    RiffReader::RiffReader(od::DataReader reader)
    : mReader(reader)
    {
        _readChunkHeader();

        if(mChunkId != RIFF_CHUNK_ID)
        {
            OD_PANIC() << "Not a RIFF file";
        }

        mParentEnd = mChunkEnd;
    }

    RiffReader::RiffReader(od::DataReader reader, std::streamoff parentEnd)
    : mReader(reader)
    , mParentEnd(parentEnd)
    , mSubchunkCount(0)
    , mSubchunkCountValid(false)
    {
        _readChunkHeader();
    }

    RiffReader::RiffReader(od::DataReader reader, std::streamoff end, std::nullptr_t)
    : mReader(reader)
    , mHasSubchunks(false)
    , mChunkLength(0)
    , mChunkStart(end)
    , mChunkEnd(end)
    , mParentEnd(end)
    , mSubchunkCount(0)
    , mSubchunkCountValid(false)
    {
    }

    RiffReader::RiffReader(od::DataReader reader, std::streamoff parentEnd, std::streamoff myStart)
    : mReader(reader)
    , mParentEnd(parentEnd)
    , mSubchunkCount(0)
    , mSubchunkCountValid(false)
    {
        mReader.seek(myStart);
        _readChunkHeader();
    }

    bool RiffReader::hasNextChunk() const
    {
        return mChunkEnd < mParentEnd;
    }

    bool RiffReader::isEnd() const
    {
        return mChunkStart >= mParentEnd;
    }

    void RiffReader::skipToNextChunk()
    {
        if(!hasNextChunk())
        {
            mChunkStart = mParentEnd;
            mChunkEnd = mParentEnd;
            return;
        }

        mReader.seek(mChunkEnd);

        _readChunkHeader();
    }

    void RiffReader::skipToNextChunkOfType(const FourCC &type, const FourCC &listType)
    {
        skipToNextChunk(); // skip at least once!
        _skipUntilTypeFound(type, listType);
    }

    void RiffReader::skipToFirstSubchunk()
    {
        if(!mHasSubchunks)
        {
            OD_PANIC() << "Chunk has no subchunks";
        }

        mReader.seek(mChunkStart + LIST_CHUNK_DATAOFFSET);
        mParentEnd = mChunkEnd;
        _readChunkHeader();
    }

    void RiffReader::skipToFirstSubchunkOfType(const FourCC &type, const FourCC &listType)
    {
        skipToFirstSubchunk();
        _skipUntilTypeFound(type, listType);
    }

    RiffReader RiffReader::getReaderForNextChunk() const
    {
        if(!hasNextChunk())
        {
            return RiffReader(mReader, mParentEnd, nullptr);
        }

        return RiffReader(mReader, mParentEnd, mChunkEnd);
    }

    RiffReader RiffReader::getReaderForFirstSubchunk() const
    {
        if(!mHasSubchunks)
        {
            OD_PANIC() << "Chunk has no subchunks";
        }

        return RiffReader(mReader, mChunkEnd, mChunkStart + LIST_CHUNK_DATAOFFSET);
    }

    RiffReader RiffReader::getReaderForNextChunkOfType(const FourCC &type, const FourCC &listType) const
    {
        RiffReader rr = getReaderForNextChunk();

        rr._skipUntilTypeFound(type, listType);

        return rr;
    }

    RiffReader RiffReader::getReaderForFirstSubchunkOfType(const FourCC &type, const FourCC &listType) const
    {
        RiffReader rr = getReaderForFirstSubchunk();

        rr._skipUntilTypeFound(type, listType);

        return rr;
    }

    od::DataReader RiffReader::getDataReader()
    {
        if(isEnd())
        {
            OD_PANIC() << "Tried to get data reader from invalid RiffReader";
        }

        if(mHasSubchunks)
        {
            mReader.seek(mChunkStart + LIST_CHUNK_DATAOFFSET);

        }else
        {
            mReader.seek(mChunkStart + REGULAR_CHUNK_DATAOFFSET);
        }

        return mReader;
    }

    size_t RiffReader::getSubchunkCount()
    {
        if(!mHasSubchunks)
        {
            return 0;
        }

        if(!mSubchunkCountValid)
        {
            RiffReader rr = getReaderForFirstSubchunk();
            while(!rr.isEnd())
            {
                rr.skipToNextChunk();
                mSubchunkCount++;
            }
            mSubchunkCountValid = true;
        }

        return mSubchunkCount;
    }

    static void _recursiveRiffPrint(std::ostream &out, RiffReader rr, int depth)
    {
        while(!rr.isEnd())
        {
            for(int i = 0 ; i < depth; ++i)
            {
                out << "   |";
            }

            if(rr.hasSubchunks())
            {
                out << "-" << rr.getChunkId() << "/" << rr.getListId() << " (length=" << rr.getChunkLength() << ")" << std::endl;
                _recursiveRiffPrint(out, rr.getReaderForFirstSubchunk(), depth+1);

            }else
            {
                out << "-" << rr.getChunkId() << " (length=" << rr.getChunkLength() << ")" << std::endl;
            }

            rr.skipToNextChunk();
        }
    }

    void RiffReader::printTree(std::ostream &out)
    {
        _recursiveRiffPrint(out, *this, 0);
    }

    void RiffReader::_readChunkHeader()
    {
        mChunkStart = mReader.tell();

        mChunkId = _readFourCC();
        mReader >> mChunkLength;

        mHasSubchunks = (mChunkId == LIST_CHUNK_ID || mChunkId == RIFF_CHUNK_ID);
        if(mHasSubchunks)
        {
            mListId = _readFourCC();

        }else
        {
            mListId = FourCC();
        }

        mSubchunkCount = 0;
        mSubchunkCountValid = false;

        size_t bytesToSkip = mChunkLength + 8; // 8 header bytes not included in length
        if(bytesToSkip & 1) ++bytesToSkip; // sizes are padded to always be even
        mChunkEnd = mChunkStart + bytesToSkip;
    }

    void RiffReader::_skipUntilTypeFound(const FourCC &type, const FourCC &listType)
    {
        if(type == LIST_CHUNK_ID || type == RIFF_CHUNK_ID)
        {
            while(!isEnd() && getListId() != listType)
            {
                skipToNextChunk();
            }

        }else
        {
            while(!isEnd() && getChunkId() != type)
            {
                skipToNextChunk();
            }
        }
    }

    FourCC RiffReader::_readFourCC()
    {
        uint32_t i;
        mReader >> i;
        return FourCC(i);
    }

}
