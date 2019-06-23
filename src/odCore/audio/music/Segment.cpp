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


    Band::Band(RiffReader rr)
    {
        if(rr.getChunkOrListId() != "DMBD") throw od::Exception("Not a Band form");

        RiffReader instrumentList = rr.getReaderForFirstSubchunkOfType("LIST", "lbil");
        for(RiffReader instrument = instrumentList.getReaderForFirstSubchunk(); !instrument.isEnd(); instrument.skipToNextChunk())
        {
            if(instrument.getChunkOrListId() != "lbin") continue;

            RiffReader header = instrument.getReaderForFirstSubchunkOfType("bins");
            if(header.isEnd()) throw od::Exception("No bins chunk in instrument");
            od::DataReader dr = header.getDataReader();

            Instrument ins;
            dr >> ins.patch
               >> ins.assignPatch
               >> ins.noteRanges[0]
               >> ins.noteRanges[1]
               >> ins.noteRanges[2]
               >> ins.noteRanges[3]
               >> ins.pChannel
               >> ins.flags
               >> ins.pan
               >> ins.volume
               >> ins.transpose
               >> ins.channelPriority
               >> ins.pitchBendRange;

            RiffReader ref = instrument.getReaderForFirstSubchunkOfType("LIST", "DMRF");
            if(ref.isEnd())
            {
                Logger::warn() << "Instrument contains no DLS reference. Ignoring instrument";
                continue;
            }
            ref.skipToFirstSubchunkOfType("guid");
            if(ref.isEnd()) throw od::Exception("No DLS GUID in instrument");

            ins.dlsGuid = Guid(ref);

            mInstruments.push_back(ins);
        }
    }


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
                _loadTracklist(rr);
            }

            rr.skipToNextChunk();
        }
    }

    void Segment::_loadHeader(RiffReader rr)
    {
        assert(rr.getChunkOrListId() == "segh");

        od::DataReader dr = rr.getDataReader();
        dr >> mNumberOfRepeats
           >> mLength
           >> mPlayStart
           >> mLoopStart
           >> mLoopEnd
           >> mResolution
           >> mRefTimeLength
           >> mFlags
           >> mReserved
           >> mRefTimeLoopStart
           >> mRefTimeLoopEnd
           >> mRefTimePlayStart;
    }

    void Segment::_loadTracklist(RiffReader trackList)
    {
        assert(trackList.getChunkOrListId() == "trkl");

        for(RiffReader track = trackList.getReaderForFirstSubchunk(); !track.isEnd(); track.skipToNextChunk())
        {
            if(track.getChunkOrListId() != "DMTK")
            {
                continue;
            }

            RiffReader header = track.getReaderForFirstSubchunkOfType(FourCC("trkh"));
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
                _loadSequenceTrack(track.getReaderForFirstSubchunkOfType("seqt"));

            }else if(typeId == "DMBT")
            {
                _loadBandTrack(track.getReaderForFirstSubchunkOfType("RIFF", "DMBT"));

            }else if(typeId == "tetr")
            {
                _loadTempoTrack(track.getReaderForFirstSubchunkOfType(FourCC("tetr")));
            }
        }
    }

    void Segment::_loadSequenceTrack(RiffReader rr)
    {
        assert(rr.getChunkOrListId() == "seqt");

        od::DataReader dr = rr.getDataReader();

        // the seqt chunk does not conform to the RIFF specs in that it contains subchunks even though it is not a LIST.
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

            mMidiCurves.push_back(curve);
        }
    }

    void Segment::_loadBandTrack(RiffReader rr)
    {
        assert(rr.getChunkOrListId() == "DMBT");

        rr.skipToFirstSubchunkOfType(FourCC("LIST"), FourCC("lbdl"));
        if(rr.isEnd()) throw od::Exception("No lbdl subchunk in band list");

        for(RiffReader bandItem = rr.getReaderForFirstSubchunk(); !bandItem.isEnd(); bandItem.skipToNextChunk())
        {
            if(bandItem.getListId() != "lbnd")
            {
                continue;
            }

            bool newStyleHeader = false;
            RiffReader header = bandItem.getReaderForFirstSubchunkOfType(FourCC("bdih"));
            if(header.isEnd())
            {
                header = bandItem.getReaderForFirstSubchunkOfType(FourCC("bd2h"));
                if(header.isEnd()) throw od::Exception("No band header in band form (neither bdih nor bd2h)");
                newStyleHeader = true;
            }

            od::DataReader dr = header.getDataReader();
            if(newStyleHeader) dr >> od::DataReader::Ignore(4);
            music_time_t bandTime;
            dr >> bandTime;

            mBandEvents.emplace_back();
            BandEvent &bandEvent = mBandEvents.back();

            RiffReader band = bandItem.getReaderForFirstSubchunkOfType(FourCC("RIFF"), FourCC("DMBD"));
            bandEvent.band = std::make_unique<Band>(band);
        }
    }

    void Segment::_loadTempoTrack(RiffReader rr)
    {
        assert(rr.getChunkOrListId() == "tetr");

        od::DataReader dr = rr.getDataReader();

        uint32_t bytesPerEvent;
        dr >> bytesPerEvent;

        assert(bytesPerEvent >= TempoEvent::STRUCT_BYTES);

        size_t eventCount = rr.getChunkLength()/bytesPerEvent;
        size_t trailingBytesPerEvent = bytesPerEvent - TempoEvent::STRUCT_BYTES;

        mTempoEvents.reserve(eventCount);
        for(size_t i = 0; i < eventCount; ++i)
        {
           TempoEvent event;
           dr >> event.time
              >> od::DataReader::Ignore(4) // TODO: seems like I have to hardcode alignment. does that mean I can assume a fixed bytesPerEvent?
              >> event.tempo
              >> od::DataReader::Ignore(trailingBytesPerEvent-4);

           mTempoEvents.push_back(event);
        }
    }

}
