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

#include <odCore/audio/SoundSystem.h>
#include <odCore/audio/Buffer.h>

namespace odDb
{

	Sound::Sound(AssetProvider &ap, od::RecordId id)
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

	Sound::~Sound()
	{
	}

    void Sound::loadFromRecord(od::DataReader &dr)
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
            throw od::UnsupportedException("Unsupported channel count");
        }

        if(mBits != 8 && mBits != 16)
        {
        	throw od::UnsupportedException("Unsupported bit count per sample");
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

    od::RefPtr<odAudio::Buffer> Sound::getOrCreateAudioBuffer(odAudio::SoundSystem *soundSystem)
    {
        if(!mSoundBuffer.isNull())
        {
            return mSoundBuffer.aquire();
        }

        if(soundSystem == nullptr)
        {
            throw od::Exception("Passed nullptr as soundSystem to Sound");
        }

        auto buffer = soundSystem->createBuffer(this);
        mSoundBuffer = buffer.get();
        return buffer;
    }

}


