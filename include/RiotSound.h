/*
 * RiotSound.h
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RIOTSOUND_H_
#define INCLUDE_RIOTSOUND_H_

#include <vector>

#include "RiotAsset.h"

#define OD_SOUND_FLAG_FLUSH_AFTER_PLAYING 	0x04
#define OD_SOUND_FLAG_PLAY_LOOPING			0x08

namespace od
{

	class RiotSound : public RiotAsset
	{
	public:

		RiotSound();
		virtual ~RiotSound();

		virtual void loadFromRecord(SrscFile &srscFile, RecordId id);

		virtual std::string getName();

		uint32_t getFlags();



	private:

		std::string mSoundName;
		uint32_t	mFlags;
        uint16_t    mChannels;
        uint16_t    mBits;
        uint32_t    mFrequency;
        uint32_t    mVolume; // no idea how this is encoded
        float       mDropoff; // min 0, max 30
        uint32_t    mPriority; // range 0-10
        uint32_t    mDecompressedSize;
        uint32_t 	mCompressionLevel; // 0 = none, 1 = lowest, 9 = highest
        uint32_t    mCompressedSize; // contains garbage if uncompressed

        std::vector<int16_t> mPcmBuffer;
	};

}


#endif /* INCLUDE_RIOTSOUND_H_ */
