/*
 * Segment.cpp
 *
 *  Created on: Jun 15, 2019
 *      Author: zal
 */

#include <odCore/audio/music/Segment.h>

#include <cassert>

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

        // the seqt chunk does not conform to the RIFF specs in that it contains subchunks even though it is not a LIST
        // since I don't want to add more methods to RiffReader to handle this sole exception, we read it like normal data
        // and pray that evtl always comes before curl

        uint32_t eventListBytes;
        uint32_t bytesPerEvent;
        dr >> od::DataReader::Expect<uint32_t>(0x6c747665) // "evtl" as LE-int
           >> eventListBytes
           >> bytesPerEvent;

        assert(bytesPerEvent >= MidiEvent::STRUCT_BYTES);

        size_t eventCount = eventListBytes/bytesPerEvent;
        size_t trailingBytesPerEvent = bytesPerEvent - MidiEvent::STRUCT_BYTES;

        mMidiEvents.reserve(eventCount);
        for(size_t i = 0; i < eventCount; ++i)
        {
            MidiEvent event;
            dr >> event.startTime
               >> event.duration
               >> event.channelIndex
               >> event.timeOffset
               >> event.midiStatus
               >> event.midiData1
               >> event.midiData2
               >> od::DataReader::Ignore(trailingBytesPerEvent);

            mMidiEvents.push_back(event);
        }

        if(eventListBytes & 1)
        {
            dr >> od::DataReader::Ignore(1);
        }

        // now load curves
        uint32_t curveListBytes;
        uint32_t bytesPerCurve;
        dr >> od::DataReader::Expect<uint32_t>(0x6c727563) // "curl" as LE-int
           >> curveListBytes
           >> bytesPerCurve;

        assert(bytesPerCurve >= MidiCurve::STRUCT_BYTES);

        size_t curveCount = curveListBytes/bytesPerCurve;
        size_t trailingBytesPerCurve = curveCount - MidiCurve::STRUCT_BYTES;

        mMidiCurves.reserve(curveCount);
        for(size_t i = 0; i < eventCount; ++i)
        {
            MidiCurve curve;
            dr >> curve.startTime
               >> curve.duration
               >> curve.resetDuration
               >> curve.channelIndex
               >> curve.timeOffset
               >> curve.startValue
               >> curve.endValue
               >> curve.resetValue
               >> curve.type
               >> curve.curveShape
               >> curve.ccData
               >> curve.flags
               >> curve.paramType
               >> curve.mergeIndex
               >> od::DataReader::Ignore(trailingBytesPerCurve);

            Logger::info() << std::hex << (uint32_t)curve.type;

            mMidiCurves.push_back(curve);
        }
    }

    void Segment::_loadBandTrack(RiffReader rr)
    {

    }

}
