/*
 * Animation.cpp
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#include <odCore/db/Animation.h>

#include <glm/mat3x4.hpp>

#include <odCore/Exception.h>

namespace odDb
{

	Animation::Animation(AssetProvider &ap, od::RecordId id)
	: Asset(ap, id)
	, mDuration(0)
	, mOriginalFrameCount(0)
	, mFrameCount(0)
	, mModelNodeCount(0)
	, mModelChannelCount(0)
	, mReferenceCount(0)
	, mRotationThreshold(0)
	, mScaleThreshold(0)
	, mTranslationThreshold(0)
	, mIsLooping(false)
	{
	}

	void Animation::loadInfo(od::DataReader &&dr)
	{
		uint32_t flags;
		uint32_t unk1;

		dr >> mAnimationName
		   >> mDuration
           >> mOriginalFrameCount
           >> mFrameCount
		   >> flags
           >> mModelNodeCount
           >> mModelChannelCount
		   >> unk1
           >> mReferenceCount
           >> mRotationThreshold
           >> mScaleThreshold
           >> mTranslationThreshold;

		mIsLooping = !(flags & 0x01);
	}

	void Animation::loadFrames(od::DataReader &&dr)
	{
		uint16_t frameCount;
		dr >> frameCount;

		mKeyframes.reserve(frameCount);
		for(size_t i = 0; i < frameCount; ++i)
		{
		    glm::mat3x4 xform3x4;

			AnimationKeyframe kf;
			dr >> kf.time
			   >> xform3x4;

			kf.xform = glm::mat4(xform3x4);

			mKeyframes.push_back(kf);
		}
	}

	void Animation::loadFrameLookup(od::DataReader &&dr)
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
		if(nodeId < 0 || (size_t)nodeId >= mFrameLookup.size())
		{
			throw od::Exception("Animation has no keyframes for requested node");
		}

		uint32_t firstFrameIndex = mFrameLookup[nodeId].first;
		uint32_t frameCount = mFrameLookup[nodeId].second;

		if(firstFrameIndex + frameCount  > mKeyframes.size())
		{
			Logger::error() << "Frame index " << (firstFrameIndex + frameCount) << " in lookup table of animation '" << mAnimationName << "' out of bounds";
			throw od::Exception("Frame lookup entry in animation is out of bounds");
		}

		return AnimStartEndPair(mKeyframes.cbegin() + firstFrameIndex, mKeyframes.cbegin() + firstFrameIndex + frameCount);
	}

}
