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
                _loadTracklist(rr);
            }

            rr.skipToNextChunk();
        }
    }

    void Segment::_loadHeader(RiffReader rr)
    {

    }

    void Segment::_loadTracklist(RiffReader rr)
    {
        RiffReader tl = rr.getReaderForFirstSubchunk();
        while(!tl.isEnd())
        {
            if(tl.getChunkOrListId() == "DMTK")
            {
                RiffReader header = tl.getReaderForFirstSubchunkOfType(FourCC("trkh"));
                if(header.isEnd()) throw od::Exception("Segment track has no header");
                od::DataReader dr = header.getDataReader();
                uint32_t headerType;
                dr >> od::DataReader::Ignore(24) // FIXME: not always correct
                   >> headerType;

                FourCC typeId(headerType);
            }

            tl.skipToNextChunk();
        }
    }

}
