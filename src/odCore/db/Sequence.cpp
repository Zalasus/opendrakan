/*
 * Sequence.cpp
 *
 *  Created on: 28 Feb 2018
 *      Author: zal
 */

#include <odCore/db/Sequence.h>

#include <algorithm>

#include <odCore/Units.h>

namespace odDb
{

    Action::Action(float t)
    : timeOffset(t)
    {
    }


    ActionTransform::ActionTransform(float t, od::DataReader &dr)
    : Action(t)
    {
        dr >> rotation
           >> position
           >> relativeActorId
           >> options;

        rotation = glm::conjugate(rotation);
        position = od::Units::worldUnitsToLengthUnits(position);
    }

    ActionTransform::InterpolationType ActionTransform::getInterpolationType() const
    {
        switch(options & 0xf0)
        {
        case 0x00:
            return InterpolationType::NONE;

        case 0x10:
            return InterpolationType::LINEAR_LINEAR;

        case 0x20:
            return InterpolationType::LINEAR_SPLINE;

        case 0x30:
            return InterpolationType::SINE_SPLINE;

        default:
            throw od::Exception("Invalid interpolation type");
        }
    }

    ActionTransform::RelativeTo ActionTransform::getRelativeTo() const
    {
        switch(options & 0x0f)
        {
        case 0x00:
            return RelativeTo::WORLD;

        case 0x01:
            return RelativeTo::ACTOR;

        case 0x02:
            return RelativeTo::LOOK_AT_ACTOR;

        default:
            throw od::Exception("Invalid relative-to type");
        }
    }


    ActionStartAnim::ActionStartAnim(float t, od::DataReader &dr)
    : Action(t)
    {
        dr >> channelIndex
           >> animationRef
           >> od::DataReader::Ignore(4)
           >> transitionTime
           >> speed
           >> rootNodeTranslationFlags;
    }

    odAnim::AxesAccumulationModes ActionStartAnim::getRootNodeTranslationModes()
    {
        auto bitsToMode = [this](int bits)
        {
            switch((rootNodeTranslationFlags >> bits) & 0x03)
            {
            case 0x00:
                return odAnim::AccumulationMode::BONE;

            case 0x01:
                return odAnim::AccumulationMode::ACCUMULATE;

            case 0x02:
                return odAnim::AccumulationMode::IGNORE;

            default:
                throw od::Exception("Invalid root node translation flags");
            }
        };

        return { bitsToMode(0), bitsToMode(2), bitsToMode(4) };
    }


    ActionPlaySound::ActionPlaySound(float t, od::DataReader &dr)
    : Action(t)
    {
        dr >> soundRef
           >> duration
           >> volume
           >> flags;
    }


    ActionAttach::ActionAttach(float t, od::DataReader &dr)
    : Action(t)
    {
        dr >> targetObjectId
           >> thisActorChannelId;
    }


    ActionRunStopAi::ActionRunStopAi(float t, od::DataReader &dr)
    : Action(t)
    {
        dr >> enableAi;
    }


    ActionShowHide::ActionShowHide(float t, od::DataReader &dr)
    : Action(t)
    {
        dr >> visible;
    }


    ActionMessage::ActionMessage(float t, od::DataReader &dr)
    : Action(t)
    {
        dr >> messageCode
           >> od::DataReader::Expect<uint32_t>(0);
    }


    ActionMusic::ActionMusic(float t, od::DataReader &dr)
    : Action(t)
    {
        dr >> musicId
           >> dlsId
           >> fadeInTime
           >> fadeOutTime;
    }


    SequenceActor::SequenceActor()
    : mActorId(0)
    , mLevelObjectId(0)
    {
    }

    void SequenceActor::load(od::DataReader &dr)
    {
        uint32_t actionCount;

        dr >> mName
           >> mActorId
           >> od::DataReader::Ignore(8)
           >> mLevelObjectId
           >> actionCount;

        mActions.reserve(actionCount);
        for(size_t i = 0; i < actionCount; ++i)
        {
            uint16_t actionType;
            float timeOffset;
            dr >> actionType
               >> timeOffset;

            switch(actionType)
            {
            case static_cast<uint16_t>(ActionType::TRANSFORM):
                mActions.emplace_back(ActionTransform(timeOffset, dr));
                break;

            case static_cast<uint16_t>(ActionType::START_ANIM):
                mActions.emplace_back(ActionStartAnim(timeOffset, dr));
                break;

            case static_cast<uint16_t>(ActionType::PLAY_SOUND):
                mActions.emplace_back(ActionPlaySound(timeOffset, dr));
                break;

            case static_cast<uint16_t>(ActionType::ATTACH):
                mActions.emplace_back(ActionAttach(timeOffset, dr));
                break;

            case static_cast<uint16_t>(ActionType::RUN_STOP_AI):
                mActions.emplace_back(ActionRunStopAi(timeOffset, dr));
                break;

            case static_cast<uint16_t>(ActionType::SHOW_HIDE):
                mActions.emplace_back(ActionShowHide(timeOffset, dr));
                break;

            case static_cast<uint16_t>(ActionType::MESSAGE):
                mActions.emplace_back(ActionMessage(timeOffset, dr));
                break;

            case static_cast<uint16_t>(ActionType::MUSIC):
                mActions.emplace_back(ActionMusic(timeOffset, dr));
                break;

            default:
                // can't continue to load as we have no idea where this action ends
                Logger::error() << "Sequence '" << mName << "' contains unknown action type " << actionType;
                throw od::Exception("Unknown action type in sequence actor '" + mName + "'");
            }
        }

        auto pred = [](ActionVariant &left, ActionVariant &right) { return Action::getTimeFromVariant(left) < Action::getTimeFromVariant(right); };
        if(!std::is_sorted(mActions.begin(), mActions.end(), pred))
        {
            Logger::warn() << "Sequence actor has unsorted timeline!";
            std::sort(mActions.begin(), mActions.end(), pred);
        }
    }


    Sequence::Sequence()
    : mRunStateModifyStyle(ModifyRunStateStyle::DO_NOT_MODIFY)
    , mLooping(false)
    {
    }

	void Sequence::load(od::SrscFile::RecordInputCursor cursor)
	{
	    od::DataReader dr = cursor.getReader();

		uint32_t actorCount;
        uint32_t flags;
        uint32_t modifyRunStateCode;

		dr >> mSequenceName
           >> flags
           >> modifyRunStateCode
		   >> od::DataReader::Ignore(4)
		   >> actorCount;

        switch(modifyRunStateCode)
        {
        case static_cast<uint32_t>(ModifyRunStateStyle::DO_NOT_MODIFY):
            mRunStateModifyStyle = ModifyRunStateStyle::DO_NOT_MODIFY;
            break;

        case static_cast<uint32_t>(ModifyRunStateStyle::STOP_ALL_OBJECTS):
            mRunStateModifyStyle = ModifyRunStateStyle::STOP_ALL_OBJECTS;
            break;

        case static_cast<uint32_t>(ModifyRunStateStyle::STOP_NON_ACTORS):
            mRunStateModifyStyle = ModifyRunStateStyle::STOP_NON_ACTORS;
            break;

        case static_cast<uint32_t>(ModifyRunStateStyle::STOP_ACTORS):
            mRunStateModifyStyle = ModifyRunStateStyle::STOP_ACTORS;
            break;

        default:
            throw od::Exception("Unknown modify-run-state-style");
        }

        mLooping = flags & 1;

		mActors.reserve(actorCount);
		for(size_t i = 0; i < actorCount; ++i)
		{
		    mActors.emplace_back();
            mActors.back().load(dr);
		}
	}

}
