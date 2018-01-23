/*
 * RiotSound.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include "RiotSound.h"

namespace od
{

	RiotSound::RiotSound()
	: mSoundName("")
	, mFlags(0)
	, mChannels(0)
    , mBits(0)
	, mFrequency(0)
	, mVolume(0)
	, mDropoff(0)
	, mPriority(0)
	, mDecompressedSize(0)
	, mCompressionLevel(0)
	, mCompressedSize(0)
    {
    }

	RiotSound::~RiotSound()
	{
	}

    void RiotSound::loadFromRecord(SrscFile &srscFile, RecordId id)
    {
        DataReader dr(srscFile.getStreamForRecordTypeId(0x0302, id));

        dr
			>> mSoundName
			>> mFlags
			>> mChannels
            >> mBits
			>> mFrequency
			>> mVolume
			>> mDropoff
			>> mPriority
			>> mDecompressedSize
			>> mCompressionLevel
			>> mCompressedSize;

        if(mBits != 8 && mBits != 16)
        {
        	throw Exception("Invalid bit count");
        }

        if(mChannels != 1)
        {
        	throw Exception("Only mono supported right now");
        }

        mPcmBuffer.allocate(mDecompressedSize);

        if(mCompressionLevel != 0)
        {
        	throw Exception("Can't load compressed sounds right now");
        }

        for(size_t i = 0; i < mDecompressedSize; ++i)
        {
        	if(mBits == 8)
        	{
        		uint8_t ateBit;
        		dr >> ateBit;

        		mPcmBuffer[i] = (ateBit << 8) - (1 << 15); // convert from biased unsigned 8 bit to signed 16 bit

        	}else if(mBits == 16)
        	{
        		dr >> mPcmBuffer[i];
        	}
        }
    }

    std::string RiotSound::getName()
    {
        return mSoundName;
    }


    OD_REGISTER_ASSET_TYPE(ASSET_SOUND, RiotSound, "sdb");
}


