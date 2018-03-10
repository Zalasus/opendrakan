/*
 * Sequence.cpp
 *
 *  Created on: 28 Feb 2018
 *      Author: zal
 */

#include "db/Sequence.h"

#include "Exception.h"

namespace od
{

    Action::Action(ActionType type, float timeOffset)
    : mActionType(type)
    , mTimeOffset(timeOffset)
    {
    }



    ActionTransform::ActionTransform(float timeOffset)
    : Action(ActionType::TRANSFORM, timeOffset)
    , mInterpolationType(InterpolationType::NONE)
    {
    }

    void ActionTransform::load(DataReader &dr)
    {
        uint16_t interpolationType;

        dr >> mRotation
           >> mPosition
           >> DataReader::Ignore(4)
           >> interpolationType
           >> DataReader::Ignore(2);

        mInterpolationType = static_cast<InterpolationType>(interpolationType);
    }



    ActionStartAnim::ActionStartAnim(float timeOffset)
    : Action(ActionType::START_ANIM, timeOffset)
    , mChannelIndex(0)
    , mAnimationRef(AssetRef::NULL_REF)
    , mTransitionTime(0)
    , mSpeed(0)
    , mRootNodeTranslationFlags(0)
    {
    }

    void ActionStartAnim::load(DataReader &dr)
    {
        dr >> mChannelIndex
           >> mAnimationRef
           >> DataReader::Ignore(4)
           >> mTransitionTime
           >> mSpeed
           >> mRootNodeTranslationFlags;
    }



    ActionPlaySound::ActionPlaySound(float timeOffset)
    : Action(ActionType::PLAY_SOUND, timeOffset)
    {
    }

    void ActionPlaySound::load(DataReader &dr)
    {
    }




    void Actor::load(DataReader &dr)
    {
        uint32_t actionCount;

        dr >> mName
           >> mActorId
           >> DataReader::Ignore(8)
           >> mLevelObjectId
           >> actionCount;

        mActions.reserve(actionCount);
        for(size_t i = 0; i < actionCount; ++i)
        {
            uint16_t actionType;
            float timeOffset;
            dr >> actionType
               >> timeOffset;

            switch(static_cast<ActionType>(actionType))
            {
            case ActionType::TRANSFORM:
                mActions.push_back(ActionPtr(new ActionTransform(timeOffset)));
                break;

            case ActionType::START_ANIM:
                mActions.push_back(ActionPtr(new ActionStartAnim(timeOffset)));
                break;

            default:
                // can't continue to load as we have no idea where this action ends
                throw Exception("Unknown action type in sequence actor '" + mName + "'");
            }

            mActions.back()->load(dr);
        }
    }


    Sequence::Sequence(Database &db, RecordId id)
    : Asset(db, id)
    {
    }

	void Sequence::loadFromRecord(DataReader &dr)
	{
		uint32_t actorCount;

		dr >> mSequenceName
		   >> DataReader::Ignore(12)
		   >> actorCount;

		mActors.reserve(actorCount);
		for(size_t i = 0; i < actorCount; ++i)
		{
		    ActorPtr newActor(new Actor);
		    newActor->load(dr);
		    mActors.push_back(newActor);
		}
	}

}
