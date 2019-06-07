/*
 * Animation.cpp
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#include <odCore/db/Animation.h>

#include <algorithm>
#include <limits>

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
	, mMinTime(std::numeric_limits<decltype(mMinTime)>::max())
	, mMaxTime(std::numeric_limits<decltype(mMaxTime)>::lowest())
	{
	}

    void Animation::load(od::SrscFile::RecordInputCursor cursor)
    {
        auto dirIt = cursor.getDirIterator();

        _loadInfo(cursor.getReader());

        if(!cursor.nextOfTypeId(od::SrscRecordType::ANIMATION_FRAMES, getAssetId(), 2))
        {
            throw od::Exception("Found no frames record after animation info record");
        }
        _loadFrames(cursor.getReader());

        cursor.moveTo(dirIt);
        if(!cursor.nextOfTypeId(od::SrscRecordType::ANIMATION_LOOKUP, getAssetId(), 2))
        {
            throw od::Exception("Found no lookup record after animation info record");
        }
        _loadFrameLookup(cursor.getReader());
    }

	std::pair<Animation::KfIterator, Animation::KfIterator> Animation::getKeyframesForNode(int32_t nodeId)
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

		return std::make_pair(mKeyframes.cbegin() + firstFrameIndex, mKeyframes.cbegin() + firstFrameIndex + frameCount);
	}

	std::pair<Animation::KfIterator, Animation::KfIterator> Animation::getLeftAndRightKeyframe(int32_t nodeId, float time)
    {
	    if(nodeId < 0 || (size_t)nodeId >= mFrameLookup.size())
        {
            throw od::Exception("Animation has no keyframes for requested node");
        }

	    uint32_t firstFrameIndex = mFrameLookup[nodeId].first;
        uint32_t frameCount = mFrameLookup[nodeId].second;

        uint32_t lastFrameIndex = firstFrameIndex + frameCount - 1;

	    if(frameCount == 0)
	    {
	        throw od::Exception("Frame lookup table contained node with 0 frames");

	    }else if(frameCount == 1)
	    {
	        return std::make_pair(mKeyframes.begin()+firstFrameIndex, mKeyframes.begin()+firstFrameIndex);
	    }

	    // more than one frame. handle extreme cases (time < start or time > end)
	    if(time <= mKeyframes[firstFrameIndex].time)
	    {
	        return std::make_pair(mKeyframes.begin()+firstFrameIndex, mKeyframes.begin()+firstFrameIndex);

	    }else if(time >= mKeyframes[lastFrameIndex].time) // TODO: bounds check?
	    {
	        return std::make_pair(mKeyframes.begin()+lastFrameIndex, mKeyframes.begin()+lastFrameIndex);
	    }

	    // we are somewhere within timeline. need to search for fitting frames.
	    //  for efficiency, we employ a type of binary search here (the keyframe list must be sorted by time, of course)

	    // NOTE: a binary search might actually be less efficient than using a linear search when starting at the last
	    // used keyframe! however, this gives us a clean, log-time interface for both playing forwards and backwards,
	    //  as well as allowing for fairly efficient time skips in animations should we need that later on

	    auto begin = mKeyframes.begin() + firstFrameIndex;
	    auto end = begin + frameCount;
	    auto pred = [](Keyframe &keyframe, float t){ return keyframe.time < t; };

	    auto it = std::lower_bound(begin, end, time, pred); // upper_bound??? should only be relevent for keyframes with same time
	    if(it == end || it == begin)
	    {
	        throw od::Exception("lower_bound found no valid keyframe. Did catching edge cases fail?");
	    }

	    return std::make_pair(it-1, it);
    }

	void Animation::_loadInfo(od::DataReader dr)
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

    void Animation::_loadFrames(od::DataReader dr)
    {
        uint16_t frameCount;
        dr >> frameCount;

        mKeyframes.reserve(frameCount);
        for(size_t i = 0; i < frameCount; ++i)
        {
            Keyframe kf;
            dr >> kf.time
               >> kf.xform;

            mMinTime = std::min(mMinTime, kf.time);
            mMaxTime = std::max(mMaxTime, kf.time);

            mKeyframes.push_back(kf);
        }
    }

    void Animation::_loadFrameLookup(od::DataReader dr)
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

}
