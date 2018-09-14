/*
 * RiotSound.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include <odCore/db/Sound.h>

#include <odCore/Exception.h>

namespace od
{

	Sound::Sound(AssetProvider &ap, RecordId id)
	: Asset(ap, id)
	, mSoundName("")
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

    void Sound::loadFromRecord(DataReader &dr)
    {
        dr  >> mSoundName
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
        	throw UnsupportedException("Unsupported bit count per sample");
        }

        if(mChannels != 1)
        {
        	throw UnsupportedException("Only mono supported right now");
        }

        mPcmBuffer.resize(mDecompressedSize);

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
}


