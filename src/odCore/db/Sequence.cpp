/*
 * Sequence.cpp
 *
 *  Created on: 28 Feb 2018
 *      Author: zal
 */

#include <odCore/db/Sequence.h>

#include <algorithm>

namespace odDb
{

    Action::Action(float t)
    : timeOffset(t)
    {
    }


    ActionTransform::ActionTransform(float t, od::DataReader &dr)
    : Action(t)
    {
        uint16_t interpolationTypeCode;

        dr >> rotation
           >> position
           >> od::DataReader::Ignore(4)
           >> interpolationTypeCode
           >> od::DataReader::Ignore(2);

        switch(interpolationTypeCode)
        {
        case static_cast<uint16_t>(InterpolationType::NONE):
            interpolationType = InterpolationType::NONE;
            break;

        case static_cast<uint16_t>(InterpolationType::LINEAR_LINEAR):
            interpolationType = InterpolationType::LINEAR_LINEAR;
            break;

        case static_cast<uint16_t>(InterpolationType::LINEAR_SPLINE):
            interpolationType = InterpolationType::LINEAR_SPLINE;
            break;

        case static_cast<uint16_t>(InterpolationType::SPLINE_SPLINE):
            interpolationType = InterpolationType::SPLINE_SPLINE;
            break;

        default:
            throw od::Exception("Invalid interpolation type");
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


    ActionVariant::ActionVariant(const ActionTransform &transform)
    : mType(ActionType::TRANSFORM)
    , mTransform(transform)
    {
    }

    ActionVariant::ActionVariant(const ActionStartAnim &startAnim)
    : mType(ActionType::START_ANIM)
    , mStartAnim(startAnim)
    {
    }

    ActionVariant::ActionVariant(const ActionPlaySound &playSound)
    : mType(ActionType::PLAY_SOUND)
    , mPlaySound(playSound)
    {
    }

    ActionVariant::ActionVariant(const ActionAttach &attach)
    : mType(ActionType::ATTACH)
    , mAttach(attach)
    {
    }

    ActionVariant::ActionVariant(const ActionRunStopAi &runStopAi)
    : mType(ActionType::RUN_STOP_AI)
    , mRunStopAi(runStopAi)
    {
    }

    ActionVariant::ActionVariant(const ActionShowHide &showHide)
    : mType(ActionType::SHOW_HIDE)
    , mShowHide(showHide)
    {
    }

    ActionVariant::ActionVariant(const ActionMessage &message)
    : mType(ActionType::MESSAGE)
    , mMessage(message)
    {
    }

    ActionVariant::ActionVariant(const ActionMusic &music)
    : mType(ActionType::MUSIC)
    , mMusic(music)
    {
    }

    template <>
    const Action *ActionVariant::as<Action>() const
    {
        const Action *result = nullptr;
        auto visitor = [&](const Action &action) mutable { result = &action; };
        this->visit(visitor);
        if(result == nullptr) OD_UNREACHABLE();
        return result;
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

        auto pred = [](ActionVariant &left, ActionVariant &right) { return left.as<Action>()->timeOffset < right.as<Action>()->timeOffset; };
        if(!std::is_sorted(mActions.begin(), mActions.end(), pred))
        {
            Logger::warn() << "Sequence actor has unsorted timeline!";
        }
    }


    Sequence::Sequence(AssetProvider &ap, od::RecordId id)
    : Asset(ap, id)
    {
    }

	void Sequence::load(od::SrscFile::RecordInputCursor cursor)
	{
	    od::DataReader dr = cursor.getReader();

		uint32_t actorCount;

		dr >> mSequenceName
		   >> od::DataReader::Ignore(12)
		   >> actorCount;

		mActors.reserve(actorCount);
		for(size_t i = 0; i < actorCount; ++i)
		{
		    mActors.emplace_back();
            mActors.back().load(dr);
		}
	}

}
