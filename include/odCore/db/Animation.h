/*
 * Animation.h
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_ANIMATION_H_
#define INCLUDE_DB_ANIMATION_H_

#include <vector>
#include <utility>
#include <glm/mat4x4.hpp>

#include <odCore/db/Asset.h>

namespace odDb
{

	struct AnimationKeyframe
	{
		float time;
		glm::mat4 xform;
	};

	/**
	 * An animation for a rigged character, as found in *.adb containers.
	 */
	class Animation : public Asset
	{
	public:

	    typedef std::vector<AnimationKeyframe>::const_iterator AnimKfIterator;
		typedef std::pair<AnimKfIterator, AnimKfIterator> AnimStartEndPair;

		Animation(AssetProvider &ap, od::RecordId id);

		inline std::string getName() const { return mAnimationName; }
		inline uint32_t getModelNodeCount() const { return mModelNodeCount; }
		inline bool isLooping() const { return mIsLooping; }

		void loadInfo(od::DataReader &&dr);
		void loadFrames(od::DataReader &&dr);
		void loadFrameLookup(od::DataReader &&dr);

		AnimStartEndPair getKeyframesForNode(int32_t nodeId);


	private:

		typedef std::pair<uint32_t, uint32_t> FrameLookupEntry; // first frame index, frame count

		std::string mAnimationName;
		float mDuration;
        uint32_t mOriginalFrameCount;
        uint32_t mFrameCount;
        uint32_t mModelNodeCount;
        uint32_t mModelChannelCount;
        uint32_t mReferenceCount;
        float mRotationThreshold;
        float mScaleThreshold;
        float mTranslationThreshold;

        bool mIsLooping;

        std::vector<AnimationKeyframe> mKeyframes;
        std::vector<FrameLookupEntry> mFrameLookup;
	};

	template <>
    struct AssetTraits<Animation>
    {
        static const char *name()
        {
            return "Animation";
        }
    };
}

#endif /* INCLUDE_DB_ANIMATION_H_ */
