/*
 * Animation.cpp
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#include "db/Animation.h"

#include "Exception.h"

namespace od
{

	Animation::Animation(Database &db, RecordId id)
	: Asset(db, id)
	, mDuration(0)
	, mOriginalFrameCount(0)
	, mFrameCount(0)
	, mModelNodeCount(0)
	, mModelChannelCount(0)
	, mReferenceCount(0)
	, mRotationThreshold(0)
	, mScaleThreshold(0)
	, mTranslationThreshold(0)
	{
	}

	void Animation::loadInfo(DataReader &&dr)
	{
		uint32_t unk0;
		uint32_t unk1;

		dr >> mAnimationName
		   >> mDuration
           >> mOriginalFrameCount
           >> mFrameCount
		   >> unk0
           >> mModelNodeCount
           >> mModelChannelCount
		   >> unk1
           >> mReferenceCount
           >> mRotationThreshold
           >> mScaleThreshold
           >> mTranslationThreshold;
	}

	void Animation::loadFrames(DataReader &&dr)
	{
		uint16_t frameCount;
		dr >> frameCount;

		mKeyframes.reserve(frameCount);
		for(size_t i = 0; i < frameCount; ++i)
		{
			AnimationKeyframe kf;
			dr >> kf.time
			   >> kf.xform;

			mKeyframes.push_back(kf);
		}
	}

	void Animation::loadFrameLookup(DataReader &&dr)
	{
		uint16_t entryCount;
		dr >> entryCount;

		mFrameLookup.reserve(entryCount);
		for(size_t i = 0; i < entryCount; ++i)
		{
			uint32_t firstFrameIndex;
			uint32_t frameCount;
			dr >> firstFrameIndex >> frameCount;

			mFrameLookup.push_back(std::make_pair(firstFrameIndex, frameCount));
		}
	}

	Animation::AnimStartEndPair Animation::getKeyframesForNode(int32_t nodeId)
	{
		if(nodeId >= mFrameLookup.size() || nodeId < 0)
		{
			throw Exception("Animation has no keyframes for requested node");
		}

		uint32_t firstFrameIndex = mFrameLookup[nodeId].first;
		uint32_t frameCount = mFrameLookup[nodeId].second;

		if(firstFrameIndex + frameCount  > mKeyframes.size())
		{
			Logger::error() << "Frame index " << (firstFrameIndex + frameCount) << " in lookup table of animation '" << mAnimationName << "' out of bounds";
			throw Exception("Frame lookup entry in animation is out of bounds");
		}

		return AnimStartEndPair(mKeyframes.cbegin() + firstFrameIndex, mKeyframes.cbegin() + firstFrameIndex + frameCount);
	}

}
