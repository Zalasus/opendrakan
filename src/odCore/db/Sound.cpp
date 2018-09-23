/*
 * RiotSound.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include <odCore/db/Sound.h>

#include <memory>

#include <odCore/Exception.h>
#include <odCore/ZStream.h>

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
			>> mCompressionLevel;

        uint32_t compressedSize = 0;
        if(mCompressionLevel != 0)
        {
            dr >> compressedSize;
        }

        if(mBits != 8 && mBits != 16)
        {
        	throw UnsupportedException("Unsupported bit count per sample");
        }

        std::unique_ptr<ZStream> zstr;
        if(mCompressionLevel != 0)
        {
            zstr.reset(new ZStream(dr.getStream()));
        }
        DataReader sampleReader(mCompressionLevel != 0 ? *zstr : dr.getStream());

        mPcmBuffer.resize(mDecompressedSize);
        sampleReader.read(reinterpret_cast<char*>(mPcmBuffer.data()), mDecompressedSize);
    }
}


