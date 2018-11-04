/*
 * RiotSound.h
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_SOUND_H_
#define INCLUDE_SOUND_H_

#include <vector>
#include <memory>

#include <odCore/db/Asset.h>

#define OD_SOUND_FLAG_FLUSH_AFTER_PLAYING 	0x04
#define OD_SOUND_FLAG_PLAY_LOOPING			0x08

namespace odDb
{

	class Sound : public Asset
	{
	public:

		Sound(AssetProvider &ap, od::RecordId id);

		inline uint32_t getSamplingFrequency() const { return mFrequency; }
		inline const std::string &getName() const { return mSoundName; }

		void loadFromRecord(od::DataReader &dr);

		float getLinearGain() const;


	private:

		std::string mSoundName;
		uint32_t	mFlags;
        uint16_t    mChannels;
        uint16_t    mBits;
        uint32_t    mFrequency;
        int32_t     mVolume; // 0 == full volume, -5000 == silent (millibel). Amplitude factor = 10^(volume / 100 / 20)
        float       mDropoff; // min 0, max 30
        uint32_t    mPriority; // range 0-10
        uint32_t    mDecompressedSize;
        uint32_t 	mCompressionLevel; // 0 = none, 1 = lowest, 9 = highest

        // temporary buffer for holding the sample data until \c mSoundBuffer is created for the sound system
        std::vector<uint8_t> mTempSampleBuffer;
	};

	template <>
    struct AssetTraits<Sound>
    {
        static const char *name()
        {
            return "Sound";
        }
    };

}


#endif /* INCLUDE_SOUND_H_ */
