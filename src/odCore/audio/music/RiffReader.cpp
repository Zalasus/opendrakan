/*
 * RiffReader.cpp
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#include <odCore/audio/music/RiffReader.h>

#include <cassert>

#include <odCore/Exception.h>

namespace odAudio
{

    RiffReader::RiffReader(od::DataReader reader)
    : mReader(reader)
    {
        _readChunkHeader();

        if(mChunkId != RIFF_CHUNK_ID)
        {
            throw RiffException("Not a RIFF file");
        }

        mParentEnd = mChunkEnd;
    }

    RiffReader::RiffReader(od::DataReader reader, std::streamoff parentEnd)
    : mReader(reader)
    , mParentEnd(parentEnd)
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
    {
    }

    bool RiffReader::hasNextChunk()
    {
        return mChunkEnd < mParentEnd;
    }

    bool RiffReader::isEnd()
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

    void RiffReader::skipToNextChunkOfType(const std::string &type, const std::string &listType)
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

    void RiffReader::skipToFirstSubchunk()
    {
        if(!mHasSubchunks)
        {
            throw RiffException("Chunk has no subchunks");
        }

        mReader.seek(mChunkStart + LIST_CHUNK_DATAOFFSET);
        mParentEnd = mChunkEnd;
        _readChunkHeader();
    }

    RiffReader RiffReader::getReaderForNextChunk()
    {
        if(!hasNextChunk())
        {
            return RiffReader(mReader, mParentEnd, nullptr);
        }

        mReader.seek(mChunkEnd);
        return RiffReader(mReader, mParentEnd);
    }

    RiffReader RiffReader::getReaderForFirstSubchunk()
    {
        if(!mHasSubchunks)
        {
            throw RiffException("Chunk has no subchunks");
        }

        mReader.seek(mChunkStart + LIST_CHUNK_DATAOFFSET);
        return RiffReader(mReader, mChunkEnd);
    }

    RiffReader RiffReader::getReaderForNextChunkOfType(const std::string &type, const std::string &listType)
    {
        RiffReader rr = getReaderForNextChunk();

        while(!rr.isEnd() && rr.getChunkId() != type && rr.getListId() != listType)
        {
            rr.skipToNextChunk();
        }

        return rr;
    }

    RiffReader RiffReader::getReaderForFirstSubchunkOfType(const std::string &type, const std::string &listType)
    {
        RiffReader rr = getReaderForFirstSubchunk();

        while(!rr.isEnd() && rr.getChunkId() != type && rr.getListId() != listType)
        {
            rr.skipToNextChunk();
        }

        return rr;
    }

    od::DataReader RiffReader::getDataReader()
    {
        if(mHasSubchunks)
        {
            mReader.seek(mChunkStart + LIST_CHUNK_DATAOFFSET);

        }else
        {
            mReader.seek(mChunkStart + REGULAR_CHUNK_DATAOFFSET);
        }

        return mReader;
    }

    static void _recursiveRiffPrint(std::ostream &out, odAudio::RiffReader rr, int depth)
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
            mListId.clear();
        }

        size_t bytesToSkip = mChunkLength + 8; // 8 header bytes not included in length
        if(bytesToSkip & 1) ++bytesToSkip; // sizes are padded to always be even
        mChunkEnd = mChunkStart + bytesToSkip;
    }

    std::string RiffReader::_readFourCC()
    {
        char c[5] = {0};
        mReader.read(&c[0], 4);

        return std::string(&c[0]);
    }

}

