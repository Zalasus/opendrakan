/*
 * Sequence.h
 *
 *  Created on: 28 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_SEQUENCE_H_
#define INCLUDE_DB_SEQUENCE_H_

#include <memory>
#include <osg/Quat>
#include <osg/Vec3f>
#include <osg/Referenced>
#include <osg/ref_ptr>

#include "db/Asset.h"

namespace od
{

    enum class ActionType : uint16_t
    {
        TRANSFORM   = 0,
        START_ANIM  = 1,
        PLAY_SOUND  = 3,
        ATTACH      = 4,
        RUN_STOP_AI = 5,
        SHOW_HIDE   = 6,
        MESSAGE     = 7
    };

    enum class InterpolationType : uint16_t
    {
        NONE = 0,
        LINEAR_LINEAR = 0x10,
        LINEAR_SPLINE = 0x20,
        SPLINE_SPLINE = 0x30
    };

    class Action
    {
    public:

        Action(ActionType type, float timeOffset);
        virtual ~Action() = default;

        inline ActionType getActionType() const { return mActionType; }
        inline float getTimeOffset() const { return mTimeOffset; }

        virtual void load(DataReader &dr) = 0;

    protected:

        ActionType mActionType;
        float mTimeOffset;
    };


    typedef std::shared_ptr<Action> ActionPtr;


    class ActionTransform : public Action
    {
    public:

        ActionTransform(float timeOffset);

        virtual void load(DataReader &dr) override;


    private:

        osg::Quat  mRotation;
        osg::Vec3f mPosition;
        InterpolationType mInterpolationType;
    };


    class ActionStartAnim : public Action
    {
    public:

        ActionStartAnim(float timeOffset);

        virtual void load(DataReader &dr) override;


    private:

        uint32_t mChannelIndex;
        AssetRef mAnimationRef;
        float mTransitionTime;
        float mSpeed;
        uint32_t mRootNodeTranslationFlags;
    };

    class ActionPlaySound : public Action
    {
    public:

        ActionPlaySound(float timeOffset);

        virtual void load(DataReader &dr) override;
    };


    class Actor
    {
    public:

        void load(DataReader &dr);


    private:

        std::string mName;
        uint32_t mActorId;
        uint32_t mLevelObjectId;
        std::vector<ActionPtr> mActions;
    };

    typedef std::shared_ptr<Actor> ActorPtr;

	class Sequence : public Asset, public osg::Referenced
	{
	public:

	    Sequence(Database &db, RecordId id);

		virtual void loadFromRecord(DataReader &dr) override;

		virtual const char *getAssetTypeName() const override { return "sequence"; }


	private:

		std::string mSequenceName;
		std::vector<ActorPtr> mActors;
	};

	typedef osg::ref_ptr<Sequence> SequencePtr;
}

#endif /* INCLUDE_DB_SEQUENCE_H_ */
