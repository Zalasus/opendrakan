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

#include <glm/mat3x4.hpp>

#include <odCore/db/Asset.h>

namespace odDb
{

	/**
	 * An animation for a rigged character, as found in *.adb containers.
	 */
	class Animation : public Asset
	{
	public:

	    struct Keyframe
        {
            float time;
            glm::mat3x4 xform;
        };

	    typedef std::vector<Keyframe>::const_iterator KfIterator;
	    typedef std::pair<KfIterator, KfIterator> KfIteratorPair;

		Animation(AssetProvider &ap, od::RecordId id);

		inline std::string getName() const { return mAnimationName; }
		inline uint32_t getModelNodeCount() const { return mModelNodeCount; }
		inline bool isLooping() const { return mIsLooping; }
		inline float getDuration() const { return mDuration; } ///< As stored in the record
		inline float getMinTime() const { return mMinTime; } ///< As calculated from the keyframes
		inline float getMaxTime() const { return mMaxTime; } ///< As calculated from the keyframes

		virtual void load(od::SrscFile::RecordInputCursor cursor) override;

		std::pair<KfIterator, KfIterator> getKeyframesForNode(int32_t nodeId);

		/**
		 * @brief Fetches iterators for the two keyframes left and right of a timepoint in a single node's timeline.
		 *
		 * If the passed timepoint lies before or after all keyframes in the node's timeline, or if the node
		 * only specifies a single keyframe, both entries in the returned pair will be identical to the first,
		 * last, or only keyframe in the timeline, respectively.
		 *
		 * If the passed node does not exist in this animation, the method will throw.
		 */
		std::pair<KfIterator, KfIterator> getLeftAndRightKeyframe(int32_t nodeId, float time);


	private:

		typedef std::pair<uint32_t, uint32_t> FrameLookupEntry; // first frame index, frame count

		void _loadInfo(od::DataReader dr);
        void _loadFrames(od::DataReader dr);
        void _loadFrameLookup(od::DataReader dr);

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

        std::vector<Keyframe> mKeyframes;
        std::vector<FrameLookupEntry> mFrameLookup;

        float mMinTime;
        float mMaxTime;
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
