/*
 * RiotSound.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include <odCore/db/Sound.h>

#include <memory>
#include <cmath>

#include <odCore/Panic.h>
#include <odCore/ZStream.h>

#include <odCore/audio/SoundSystem.h>
#include <odCore/audio/Buffer.h>

namespace odDb
{

	Sound::Sound()
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
    {
    }

	Sound::~Sound()
	{
	}

    void Sound::load(od::SrscFile::RecordInputCursor cursor)
    {
        od::DataReader dr = cursor.getReader();

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

        if(mChannels != 1 && mChannels != 2)
        {
            OD_PANIC() << "Unsupported channel count " << mChannels;
        }

        if(mBits != 8 && mBits != 16)
        {
        	OD_PANIC() << "Unsupported bit count per sample " << mBits;
        }

        std::unique_ptr<od::ZStream> zstr;
        od::DataReader sampleReader(dr);
        if(mCompressionLevel != 0)
        {
            size_t outputBufferSize = std::min(od::ZStreamBuffer::DefaultBufferSize, (size_t)mDecompressedSize);
            zstr = std::make_unique<od::ZStream>(dr.getStream(), compressedSize, outputBufferSize);
            sampleReader.setStream(*zstr);
        }

        mDataBuffer.resize(mDecompressedSize);
        sampleReader.read(reinterpret_cast<char*>(mDataBuffer.data()), mDecompressedSize);
    }

    float Sound::getLinearGain() const
    {
        return std::pow(10.0f, mVolume/2000.0f);
    }

    float Sound::getDuration() const
    {
        size_t bytesPerSample = mChannels*(mBits/8);
        size_t sampleCount = mDataBuffer.size() / bytesPerSample;
        return static_cast<float>(sampleCount * 1.0f/mFrequency);
    }
}
