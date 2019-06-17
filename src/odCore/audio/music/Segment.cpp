/*
 * Segment.cpp
 *
 *  Created on: Jun 15, 2019
 *      Author: zal
 */

#include <odCore/audio/music/Segment.h>

#include <odCore/Logger.h>

namespace odAudio
{

    Segment::Segment(RiffReader rr)
    {
        _load(rr);
    }

    void Segment::_load(RiffReader rr)
    {
        if(rr.getChunkOrListId() != "DMSG")
        {
            throw od::Exception("Not a DirectMusic segment");
        }

        rr.skipToFirstSubchunk();

        while(!rr.isEnd())
        {
            /*switch(rr.getChunkOrListId())
            {
            case FourCC("guid"):
                mGuid = Guid(rr);
                break;

            }*/

            if(rr.getChunkId() == "guid")
            {
                mGuid = Guid(rr);

            }else if(rr.getChunkId() == "segh")
            {
                _loadHeader(rr);

            }else if(rr.getChunkOrListId() == "trkl")
            {
                _loadTracklist(rr.getReaderForFirstSubchunk());
            }

            rr.skipToNextChunk();
        }
    }

    void Segment::_loadHeader(RiffReader rr)
    {

    }

    void Segment::_loadTracklist(RiffReader rr)
    {
        while(!rr.isEnd())
        {
            if(rr.getChunkOrListId() == "DMTK")
            {
                RiffReader header = rr.getReaderForFirstSubchunkOfType(FourCC("trkh"));
                if(header.isEnd()) throw od::Exception("Segment track has no header");
                od::DataReader dr = header.getDataReader();
                uint32_t headerType;
                uint32_t headerListType;
                dr >> od::DataReader::Ignore(24)
                   >> headerType
                   >> headerListType;

                FourCC typeId(headerType != 0 ? headerType : headerListType);

                if(typeId == "seqt")
                {
                    _loadSequenceTrack(rr.getReaderForFirstSubchunkOfType(FourCC("seqt")));
                }
            }

            rr.skipToNextChunk();
        }
    }

    void Segment::_loadSequenceTrack(RiffReader rr)
    {
        od::DataReader dr = rr.getDataReader();

        static const uint32_t EVENT_BYTE_COUNT = 20; // size of _one_ event struct in bytes

        // the seqt chunk does not conform to the RIFF specs in that it contains subchunks even though it is not a LIST
        // since I don't want to add more methods to RiffReader to handle this sole exception, we read it like normal data
        // and pray that evtl always comes before curl

        uint32_t eventListBytes;
        dr >> od::DataReader::Expect<uint32_t>(0x6c747665) // "evtl" as LE-int
           >> eventListBytes
           >> od::DataReader::Expect<uint32_t>(EVENT_BYTE_COUNT); // this field should never be anything but the documented length

        size_t eventCount = eventListBytes/EVENT_BYTE_COUNT;
        if(eventListBytes != eventCount*EVENT_BYTE_COUNT)
        {
            Logger::warn() << "Stray bytes in event list";
        }

        Logger::info() << "Event count: " << eventCount;

        mMidiEvents.reserve(eventCount);
        for(size_t i = 0; i < eventCount; ++i)
        {
            MidiEvent event;
            dr >> event.startTime
               >> event.duration
               >> event.channelIndex
               >> event.command
               >> event.commandArg1
               >> event.commandArg2
               >> od::DataReader::Ignore(5);

            mMidiEvents.push_back(event);
        }
    }

}
