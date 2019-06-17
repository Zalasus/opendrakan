/*
 * RiffReader.h
 *
 *  Created on: 11 Jun 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_RIFFREADER_H_
#define INCLUDE_ODCORE_AUDIO_RIFFREADER_H_

#include <ostream>

#include <odCore/DataStream.h>
#include <odCore/Exception.h>

namespace odAudio
{
    class FourCC
    {
    public:

        FourCC();
        /**
         * @brief Creates a FourCC from an integer that has been read as little-endian.
         */
        explicit FourCC(uint32_t i);
        explicit FourCC(const char *c);
        FourCC(char c0, char c1, char c2, char c3);

        operator uint32_t() const
        {
            return code;
        }

        bool operator==(const FourCC &f) const
        {
            return code == f.code;
        }

        bool operator==(const char *c) const
        {
            return *this == FourCC(c);
        }

        bool operator!=(const FourCC &f) const
        {
            return code != f.code;
        }

        bool operator!=(const char *c) const
        {
            return *this != FourCC(c);
        }

        uint32_t code;
    };

    std::ostream &operator<<(std::ostream &lhs, const FourCC &rhs);


    /**
     * @brief Basic chunk-level RIFF reader.
     *
     * This features an iterator-style interface for skipping through a RIFF file and descending into subchunks.
     *
     * This reader is position-agnostic. It can read RIFF data at any position in a stream.
     *
     * Only header data is read and kept in memory. Everything else must be read from the stream by the user.
     *
     * We use this to extract the DLS and segment names from the music records in Music.rrc, which contain raw RIFF data.
     */
    class RiffReader
    {
    public:

        static const FourCC RIFF_CHUNK_ID;
        static const FourCC LIST_CHUNK_ID;

        static const size_t LIST_CHUNK_DATAOFFSET = 12;
        static const size_t REGULAR_CHUNK_DATAOFFSET = 8;

        RiffReader(od::DataReader reader);

        inline bool hasSubchunks() const { return mHasSubchunks; }
        inline uint32_t getChunkLength() const { return mChunkLength; }
        inline FourCC getChunkId() const { return mChunkId; }
        inline FourCC getListId() const { return mListId; }

        /**
         * @brief Returns the chunk ID or the list ID for LIST/RIFF chunks.
         */
        inline FourCC getChunkOrListId() const { return mHasSubchunks ? mListId : mChunkId; }

        bool hasNextChunk() const;
        bool isEnd() const;

        void skipToNextChunk();
        void skipToNextChunkOfType(const FourCC &type, const FourCC &listType = FourCC());
        void skipToFirstSubchunk();
        void skipToFistSubchunkOfType(const FourCC &type, const FourCC &listType = FourCC());

        RiffReader getReaderForNextChunk();
        RiffReader getReaderForNextChunkOfType(const FourCC &type, const FourCC &listType = FourCC());
        RiffReader getReaderForFirstSubchunk();
        RiffReader getReaderForFirstSubchunkOfType(const FourCC &type, const FourCC &listType = FourCC());

        od::DataReader getDataReader();

        void printTree(std::ostream &out);


    private:

        RiffReader(od::DataReader reader, std::streamoff end, std::nullptr_t);
        RiffReader(od::DataReader reader, std::streamoff parentEnd);

        void _readChunkHeader();
        void _skipUntilTypeFound(const FourCC &type, const FourCC &listType);
        FourCC _readFourCC();

        od::DataReader mReader;

        bool mHasSubchunks;
        uint32_t mChunkLength;
        FourCC mChunkId;
        FourCC mListId;

        std::streamoff mChunkStart;
        std::streamoff mChunkEnd;
        std::streamoff mParentEnd;
    };


    class RiffException : public od::Exception
    {
    public:

        RiffException(const std::string &msg)
        : Exception(msg)
        {
        }

    };

}
#endif /* INCLUDE_ODCORE_AUDIO_RIFFREADER_H_ */
