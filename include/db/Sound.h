/*
 * RiotSound.h
 *
 *  Created on: 19 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_SOUND_H_
#define INCLUDE_SOUND_H_

#include <vector>
#include <osg/Referenced>

#include "Asset.h"

#define OD_SOUND_FLAG_FLUSH_AFTER_PLAYING 	0x04
#define OD_SOUND_FLAG_PLAY_LOOPING			0x08

namespace od
{

	class Sound : public Asset, public osg::Referenced
	{
	public:

		Sound(Database &db, RecordId id);

		virtual void loadFromRecord(DataReader &dr) override;

		// implement Asset
        virtual const char *getAssetTypeName() const override { return "sound"; }


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


#endif /* INCLUDE_SOUND_H_ */
