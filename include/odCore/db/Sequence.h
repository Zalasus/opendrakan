/*
 * Sequence.h
 *
 *  Created on: 28 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_SEQUENCE_H_
#define INCLUDE_DB_SEQUENCE_H_

#include <memory>
#include <vector>
#include <variant>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/Exception.h>

#include <odCore/anim/AnimModes.h>

#include <odCore/db/Asset.h>

namespace odDb
{

    enum class ActionType : uint16_t
    {
        TRANSFORM   = 0,
        START_ANIM  = 1,
        PLAY_SOUND  = 3,
        ATTACH      = 4,
        RUN_STOP_AI = 5,
        SHOW_HIDE   = 6,
        MESSAGE     = 7,
        MUSIC       = 8
    };


    enum class ModifyRunStateStyle
    {
        DO_NOT_MODIFY,
        STOP_ALL_OBJECTS,
        STOP_NON_ACTORS,
        STOP_ACTORS
    };


    /**
     * @brief Common base for all actions. Helps with writing visitors.
     */
    struct Action
    {
        Action(float t);

        float timeOffset;

        template <typename T>
        static float getTimeFromVariant(const T &v)
        {
            auto visitor = [](const Action &a) -> float
            {
                return a.timeOffset;
            };

            return std::visit(visitor, v);
        }
    };


    struct ActionTransform : public Action
    {
        enum class InterpolationType
        {
            NONE,
            LINEAR_LINEAR,
            LINEAR_SPLINE,
            SINE_SPLINE
        };

        enum class RelativeTo
        {
            WORLD,
            ACTOR,
            LOOK_AT_ACTOR
        };

        ActionTransform(float t, od::DataReader &dr);

        InterpolationType getInterpolationType() const;
        RelativeTo getRelativeTo() const;

        bool ignorePosition() const
        {
            return options & 0x0100;
        }

        bool ignoreRotation() const
        {
            return options & 0x0200;
        }

        glm::quat rotation;
        glm::vec3 position;
        uint32_t relativeActorId;
        uint32_t options;
    };


    struct ActionStartAnim : public Action
    {
        ActionStartAnim(float t, od::DataReader &dr);

        odAnim::AxesAccumulationModes getRootNodeTranslationModes();

        uint32_t channelIndex;
        AssetRef animationRef;
        float transitionTime;
        float speed;
        uint32_t rootNodeTranslationFlags;
    };


    struct ActionPlaySound : public Action
    {
        ActionPlaySound(float t, od::DataReader &dr);

        AssetRef soundRef;
        float duration;
        uint16_t volume; // [0..10000]
        uint16_t flags; // 0x8 == flush from memory after playing, 0x10 == mono, 0x30 == notify actor (what does this do?)
    };


    struct ActionAttach : public Action
    {
        ActionAttach(float t, od::DataReader &dr);

        uint32_t targetObjectId;
        uint32_t thisActorChannelId;
    };


    struct ActionRunStopAi : public Action
    {
        ActionRunStopAi(float t, od::DataReader &dr);

        uint32_t enableAi;
    };


    struct ActionShowHide : public Action
    {
        ActionShowHide(float t, od::DataReader &dr);

        uint32_t visible;
    };


    struct ActionMessage : public Action
    {
        ActionMessage(float t, od::DataReader &dr);

        uint32_t messageCode; // no trivial relation with class enums!
    };


    struct ActionMusic : public Action
    {
        ActionMusic(float t, od::DataReader &dr);

        uint32_t musicId;
        uint32_t dlsId;
        float fadeInTime;
        float fadeOutTime;
    };


    using ActionVariant = std::variant<ActionTransform,
                                       ActionStartAnim,
                                       ActionPlaySound,
                                       ActionAttach,
                                       ActionRunStopAi,
                                       ActionShowHide,
                                       ActionMessage,
                                       ActionMusic>;


    class SequenceActor
    {
    public:

        SequenceActor();

        inline const std::string &getName() const { return mName; }
        inline uint32_t getActorId() const { return mActorId; }
        inline uint32_t getLevelObjectId() const { return mLevelObjectId; }
        inline const std::vector<ActionVariant> &getActions() const { return mActions; }

        void load(od::DataReader &dr);


    private:

        std::string mName;
        uint32_t mActorId;
        uint32_t mLevelObjectId;
        std::vector<ActionVariant> mActions;
    };


	class Sequence : public Asset
	{
	public:

	    Sequence();

        inline const std::string &getName() const { return mSequenceName; }
        inline const std::vector<SequenceActor> &getActors() const { return mActors; }
        inline ModifyRunStateStyle getRunStateModifyStyle() const { return mRunStateModifyStyle; }

		virtual void load(od::SrscFile::RecordInputCursor cursor) override;


	private:

		std::string mSequenceName;
        ModifyRunStateStyle mRunStateModifyStyle;
        bool mLooping;
		std::vector<SequenceActor> mActors;
	};


	template <>
    struct AssetTraits<Sequence>
    {
        static const char *name()
        {
            return "Sequence";
        }

        static constexpr od::RecordType baseType()
        {
            return static_cast<od::RecordType>(od::SrscRecordType::SEQUENCE);
        }
    };

}

#endif /* INCLUDE_DB_SEQUENCE_H_ */
