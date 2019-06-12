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

        static const constexpr char *RIFF_CHUNK_ID = "RIFF";
        static const constexpr char *LIST_CHUNK_ID = "LIST";

        static const size_t LIST_CHUNK_DATAOFFSET = 12;
        static const size_t REGULAR_CHUNK_DATAOFFSET = 8;

        RiffReader(od::DataReader reader);

        inline bool hasSubchunks() const { return mHasSubchunks; }
        inline uint32_t getChunkLength() const { return mChunkLength; }
        inline std::string getChunkId() const { return mChunkId; }
        inline std::string getListId() const { return mListId; }

        bool hasNextChunk();
        bool isEnd();

        void skipToNextChunk();
        void skipToNextChunkOfType(const std::string &type, const std::string &listType = "");
        void skipToFirstSubchunk();
        void skipToFistSubchunkOfType(const std::string &type, const std::string &listType = "");

        RiffReader getReaderForNextChunk();
        RiffReader getReaderForNextChunkOfType(const std::string &type, const std::string &listType = "");
        RiffReader getReaderForFirstSubchunk();
        RiffReader getReaderForFirstSubchunkOfType(const std::string &type, const std::string &listType = "");

        od::DataReader getDataReader();

        void printTree(std::ostream &out);


    private:

        RiffReader(od::DataReader reader, std::streamoff end, std::nullptr_t);
        RiffReader(od::DataReader reader, std::streamoff parentEnd);

        void _readChunkHeader();
        std::string _readFourCC();

        od::DataReader mReader;

        bool mHasSubchunks;
        uint32_t mChunkLength;
        std::string mChunkId;
        std::string mListId;

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
