/*
 * StreamingSource.h
 *
 *  Created on: Jun 19, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_AUDIO_STREAMINGSOURCE_H_
#define INCLUDE_ODOSG_AUDIO_STREAMINGSOURCE_H_

#include <functional>
#include <deque>
#include <memory>
#include <vector>

#include <odOsg/audio/Source.h>
#include <odOsg/audio/Buffer.h>

namespace odOsg
{
    class SoundSystem;

    class StreamingSource : public Source
    {
    public:


        typedef std::function<void(int16_t *buffer, size_t bufferSize)> BufferFillCallback;

        StreamingSource(SoundSystem &ss, size_t bufferCount, size_t samplesPerBufferAndChannel, bool stereo);
        virtual ~StreamingSource();

        void setBufferFillCallback(const BufferFillCallback &callback);

        virtual void setSound(std::shared_ptr<odDb::Sound> s) override;
        virtual void update(float relTime) override;


    private:

        void _fillBuffer_locked(Buffer *buffer, const StreamingSource::BufferFillCallback &callback);

        size_t mSamplesPerBuffer;
        bool mIsStereo;
        std::unique_ptr<int16_t[]> mTempFillBuffer;
        std::deque<od::RefPtr<Buffer>> mBuffers;
        std::vector<ALuint> mBufferIds;

        BufferFillCallback mFillCallback;
    };

}

#endif /* INCLUDE_ODOSG_AUDIO_STREAMINGSOURCE_H_ */
