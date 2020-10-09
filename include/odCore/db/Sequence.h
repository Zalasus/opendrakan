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
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/Exception.h>

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


    /**
     * @brief Common base for all actions. Helps with writing visitors.
     */
    struct Action
    {
        Action(float t);

        float timeOffset;
    };


    struct ActionTransform : public Action
    {
        enum class InterpolationType : uint16_t
        {
            NONE = 0,
            LINEAR_LINEAR = 0x10,
            LINEAR_SPLINE = 0x20,
            SPLINE_SPLINE = 0x30
        };

        ActionTransform(float t, od::DataReader &dr);

        glm::quat rotation;
        glm::vec3 position;
        InterpolationType interpolationType;
    };


    struct ActionStartAnim : public Action
    {
        ActionStartAnim(float t, od::DataReader &dr);

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


    class ActionVariant
    {
    public:

        ActionVariant(const ActionTransform &transform);
        ActionVariant(const ActionStartAnim &startAnim);
        ActionVariant(const ActionPlaySound &playSound);
        ActionVariant(const ActionAttach    &attach);
        ActionVariant(const ActionRunStopAi &runStopAi);
        ActionVariant(const ActionShowHide  &showHide);
        ActionVariant(const ActionMessage   &message);
        ActionVariant(const ActionMusic     &music);

        ActionType getType() const { return mType; }

        template <typename F>
        void visit(F f) const
        {
            switch(mType)
            {
            case ActionType::TRANSFORM:
                f(mTransform);
                break;

            case ActionType::START_ANIM:
                f(mStartAnim);
                break;

            case ActionType::PLAY_SOUND:
                f(mPlaySound);
                break;

            case ActionType::ATTACH:
                f(mAttach);
                break;

            case ActionType::RUN_STOP_AI:
                f(mRunStopAi);
                break;

            case ActionType::SHOW_HIDE:
                f(mShowHide);
                break;

            case ActionType::MESSAGE:
                f(mMessage);
                break;

            case ActionType::MUSIC:
                f(mMusic);
                break;
            }
        }

        template <typename T>
        const T *as() const
        {
            struct RetrieveVisitor
            {
                const T *result;

                RetrieveVisitor()
                : result(nullptr)
                {
                }

                RetrieveVisitor &operator()(const Action &a)
                {
                    return *this;
                }

                RetrieveVisitor &operator()(const T &a)
                {
                    result = &a;
                    return *this;
                }
            };

            RetrieveVisitor visitor;
            this->visit(visitor);

            return visitor.result;
        }


    private:

        ActionType mType;

        union
        {
            ActionTransform mTransform;
            ActionStartAnim mStartAnim;
            ActionPlaySound mPlaySound;
            ActionAttach    mAttach;
            ActionRunStopAi mRunStopAi;
            ActionShowHide  mShowHide;
            ActionMessage   mMessage;
            ActionMusic     mMusic;
        };

    };

    template<>
    const Action *ActionVariant::as<Action>() const;


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

	    Sequence(AssetProvider &ap, od::RecordId id);

        inline const std::string &getName() const { return mSequenceName; }
        inline const std::vector<SequenceActor> &getActors() const { return mActors; }

		virtual void load(od::SrscFile::RecordInputCursor cursor) override;


	private:

		std::string mSequenceName;
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
