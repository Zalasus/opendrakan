/*
 * RiotSound.cpp
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#include <odCore/db/Sound.h>

#include <memory>
#include <cmath>

#include <odCore/Exception.h>
#include <odCore/ZStream.h>
#include <odCore/audio/Buffer.h>

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

        if(mChannels != 1 && mChannels != 2)
        {
            throw UnsupportedException("Unsupported channel count");
        }

        if(mBits != 8 && mBits != 16)
        {
        	throw UnsupportedException("Unsupported bit count per sample");
        }

        std::unique_ptr<ZStream> zstr;
        DataReader sampleReader(dr);
        if(mCompressionLevel != 0)
        {
            size_t outputBufferSize = std::min(ZStreamBuffer::DefaultBufferSize, (size_t)mDecompressedSize);
            zstr.reset(new ZStream(dr.getStream(), compressedSize, outputBufferSize));
            sampleReader.setStream(*zstr);
        }

        mPcmBuffer.resize(mDecompressedSize);
        sampleReader.read(reinterpret_cast<char*>(mPcmBuffer.data()), mDecompressedSize);
    }

    std::shared_ptr<Buffer> Sound::getOrCreateBuffer(SoundManager &soundManager)
    {
        if(mSoundBuffer != nullptr)
        {
            return mSoundBuffer;
        }

        mSoundBuffer.reset(new Buffer(soundManager));

        Buffer::Format format = mSoundBuffer->getFormatFor(mBits, mChannels);

        mSoundBuffer->setData(mPcmBuffer.data(), mPcmBuffer.size(), format, mFrequency);

        return mSoundBuffer;
    }

    float Sound::getLinearGain() const
    {
        return std::exp(mVolume/2000.0f); //std::pow(10.0f, mVolume/2000.0f);
    }

}


