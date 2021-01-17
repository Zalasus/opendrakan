/*
 * StreamingSource.cpp
 *
 *  Created on: Jun 19, 2019
 *      Author: zal
 */

#include <odOsg/audio/StreamingSource.h>

#include <algorithm>
#include <mutex>
#include <vector>

#include <odCore/Panic.h>

#include <odOsg/audio/SoundSystem.h>

namespace odOsg
{

    static StreamingSource::BufferFillCallback FILL_WITH_SILENCE = [](int16_t *buffer, size_t bufferSize)
    {
        std::fill(buffer, buffer+bufferSize, 0);
    };


    StreamingSource::StreamingSource(SoundSystem &ss, size_t bufferCount, size_t samplesPerBufferAndChannel, bool isStereo)
    : Source(ss)
    , mSamplesPerBuffer(samplesPerBufferAndChannel * (isStereo ? 1 : 2))
    , mIsStereo(isStereo)
    , mTempFillBuffer(std::make_unique<int16_t[]>(mSamplesPerBuffer))
    , mBuffers(bufferCount, nullptr)
    {
        setBufferFillCallback(nullptr);

        mBufferIds.resize(bufferCount, 0);
        for(size_t i = 0; i < bufferCount; ++i)
        {
            mBuffers[i] = std::make_shared<Buffer>(ss);
            mBufferIds[i] = mBuffers[i]->getBufferId();
            _fillBuffer_locked(*mBuffers[i], FILL_WITH_SILENCE);
        }

        alSourceQueueBuffers(mSourceId, bufferCount, mBufferIds.data());
        SoundSystem::doErrorCheck("Could not enqueue newly created buffers into streaming source");
    }

    StreamingSource::~StreamingSource()
    {
        std::lock_guard<std::mutex> lock(getMutex());

        alSourceStop(mSourceId);
        SoundSystem::doErrorCheck("Could not stop streaming source to delete it");

        alSourceUnqueueBuffers(mSourceId, mBufferIds.size(), mBufferIds.data());
        SoundSystem::doErrorCheck("Could not enqueue newly created buffers into streaming source");
    }

    void StreamingSource::setBufferFillCallback(const BufferFillCallback &callback)
    {
        std::lock_guard<std::mutex> lock(getMutex());

        if(callback == nullptr)
        {
            mFillCallback = FILL_WITH_SILENCE;

        }else
        {
            mFillCallback = callback;
        }
    }

    void StreamingSource::setSound(std::shared_ptr<odDb::Sound> s)
    {
        OD_PANIC() << "Streaming sources can't play database sounds";
    }

    void StreamingSource::update(float relTime)
    {
        Source::update(relTime);

        ALint queuedBuffers;
        alGetSourcei(mSourceId, AL_BUFFERS_QUEUED, &queuedBuffers);
        SoundSystem::doErrorCheck("Failed to query number of queued buffers of streaming source");

        ALint processedBuffers;
        alGetSourcei(mSourceId, AL_BUFFERS_PROCESSED, &processedBuffers);
        SoundSystem::doErrorCheck("Failed to query number of processed buffers of streaming source");

        if(processedBuffers == 0)
        {
            return;

        }else if(processedBuffers == queuedBuffers)
        {
            Logger::warn() << "Streaming source underrun. Make sure you provide enough buffers or make them big enough";
        }

        // take played buffers and refill them
        for(ALint i = 0; i < processedBuffers; ++i)
        {
            auto buffer = mBuffers.front();
            mBuffers.pop_front();

            ALuint bufferId = buffer->getBufferId();

            alSourceUnqueueBuffers(mSourceId, 1, &bufferId);
            SoundSystem::doErrorCheck("Could not unqueue buffer from streaming source");

            _fillBuffer_locked(*buffer, mFillCallback);

            alSourceQueueBuffers(mSourceId, 1, &bufferId);
            SoundSystem::doErrorCheck("Could not queue buffer into streaming source");
            mBuffers.push_back(buffer);
        }
    }

    void StreamingSource::_fillBuffer_locked(Buffer &buffer, const StreamingSource::BufferFillCallback &callback)
    {
        mFillCallback(mTempFillBuffer.get(), mSamplesPerBuffer);

        ALenum format = mIsStereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
        ALsizei size = mSamplesPerBuffer*sizeof(int16_t);

        alBufferData(buffer.getBufferId(), format, mTempFillBuffer.get(), size, mSoundSystem.getContext().getOutputFrequency());
        SoundSystem::doErrorCheck("Failed to push data from fill buffer to AL buffer");
    }
}
