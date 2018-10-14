/*
 * Sequence.cpp
 *
 *  Created on: 28 Feb 2018
 *      Author: zal
 */

#include <odCore/db/Sequence.h>

#include <odCore/Exception.h>

namespace odDb
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

    void ActionTransform::load(od::DataReader &dr)
    {
        uint16_t interpolationType;

        dr >> mRotation
           >> mPosition
           >> od::DataReader::Ignore(4)
           >> interpolationType
           >> od::DataReader::Ignore(2);

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

    void ActionStartAnim::load(od::DataReader &dr)
    {
        dr >> mChannelIndex
           >> mAnimationRef
           >> od::DataReader::Ignore(4)
           >> mTransitionTime
           >> mSpeed
           >> mRootNodeTranslationFlags;
    }



    ActionPlaySound::ActionPlaySound(float timeOffset)
    : Action(ActionType::PLAY_SOUND, timeOffset)
    {
    }

    void ActionPlaySound::load(od::DataReader &dr)
    {
    }




    void Actor::load(od::DataReader &dr)
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
                throw od::Exception("Unknown action type in sequence actor '" + mName + "'");
            }

            mActions.back()->load(dr);
        }
    }


    Sequence::Sequence(AssetProvider &ap, od::RecordId id)
    : Asset(ap, id)
    {
    }

	void Sequence::loadFromRecord(od::DataReader &dr)
	{
		uint32_t actorCount;

		dr >> mSequenceName
		   >> od::DataReader::Ignore(12)
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
