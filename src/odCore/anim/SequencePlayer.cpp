
#include <odCore/anim/SequencePlayer.h>

#include <algorithm>
#include <limits>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Message.h>
#include <odCore/Panic.h>

#include <odCore/anim/SkeletonAnimationPlayer.h>
#include <odCore/anim/BoneAccumulator.h>

namespace odAnim
{

    static od::Message getMessageForCode(uint32_t code)
    {
        // this seems to serve multiple purposes. some classes like fader can
        //  be passed a numeric value via the message interface (fade time for faders).
        switch(code)
        {
        case 4098:
            return od::Message::Off;

        case 4097:
            return od::Message::On;

        case 4353:
            return od::Message::Lock;

        case 4354:
            return od::Message::Unlock;

        case 4257:
            return od::Message::PlaySequence;

        case 4259:
            return od::Message::BlowUp;

        case 4103:
            return od::Message::Triggered;

        default:
            Logger::warn() << "Unknown message code " << code << " in sequence. Need to investigate";
            return od::Message::BlowUp;
        }
    }


    SequencePlayer::SequencePlayer(od::Level &level)
    : mLevel(level)
    , mSequenceTime(0.0)
    , mSequenceDuration(0.0)
    , mLastUpdateSequenceTime(0.0)
    , mPlaying(false)
    {
    }

    // splits TF and non-TF events. also calculates end time for each event (for calculating the duration of the sequence)
    class ActionLoadVisitor
    {
    public:

        ActionLoadVisitor(SequencePlayer::Actor &actor, odDb::Sequence &sequence, od::LevelObject &object)
        : mActor(actor)
        , mSequence(sequence)
        , mObject(object)
        {
        }

        float operator()(const odDb::ActionTransform &a)
        {
            mActor.transformActions.emplace_back(a);

            return a.timeOffset;
        }

        float operator()(const odDb::ActionStartAnim &a)
        {
            // for some reason, animation refs are stored relative to the object's model's DB, not the sequence DB
            if(mObject.getModel() == nullptr)
            {
                Logger::error() << "Animation on object without loaded model. Can't load animation asset";
                return a.timeOffset;
            }
            auto animDepTable = mObject.getModel()->getDependencyTable();

            PlayerActionStartAnim newAction(a);
            newAction.animation = animDepTable->loadAsset<odDb::Animation>(a.animationRef);
            if(newAction.animation == nullptr)
            {
                Logger::error() << "Missing animation in sequence: " << a.animationRef;
                return a.timeOffset;
            }

            mActor.nonTransformActions.emplace_back(newAction);

            return a.timeOffset + newAction.animation->getDuration();
        }

        float operator()(const odDb::ActionPlaySound &a)
        {
            PlayerActionPlaySound newAction(a);
            newAction.sound = mSequence.getDependencyTable()->loadAsset<odDb::Sound>(a.soundRef);
            if(newAction.sound == nullptr)
            {
                Logger::error() << "Missing sound in sequence: " << a.soundRef;
                return a.timeOffset;
            }

            mActor.nonTransformActions.emplace_back(newAction);

            return a.timeOffset + newAction.sound->getDuration();
        }

        template <typename _OtherAction>
        float operator()(const _OtherAction &a)
        {
            mActor.nonTransformActions.emplace_back(a);

            return a.timeOffset;
        }


    private:

        SequencePlayer::Actor &mActor;
        odDb::Sequence &mSequence;

        // can't use mActor.actorObject, since at this point is is still nullptr
        //  (won't get filled until play() is called to avoid permanent
        //  reference cycles)
        od::LevelObject &mObject;

    };

    void SequencePlayer::loadSequence(std::shared_ptr<odDb::Sequence> sequence)
    {
        OD_CHECK_ARG_NONNULL(sequence);

        mSequence = sequence;
        mSequenceTime = 0.0;
        mSequenceDuration = 0.0;

        auto &actors = sequence->getActors();
        mActors.clear();
        mActors.reserve(actors.size());
        for(auto &dbActor : actors)
        {
            auto actorObject = mLevel.getLevelObjectById(dbActor.getLevelObjectId());
            if(actorObject == nullptr)
            {
                Logger::warn() << "Actor '" << dbActor.getName() << "' in sequence '" << sequence->getName() << "' has invalid object reference";
                continue;
            }

            auto objectId = actorObject->getObjectId();
            auto newActorIt = mActors.insert(std::make_pair(objectId, Actor{objectId}));
            auto &playerActor = (newActorIt.first)->second;

            // we need to split off transform and non-transform actions into their own vectors, as to make interpolation easier.
            //  we take this opportunity to prefetch animations and sounds, which are represented by alternative action variants.
            //  TODO: instead of using alternative actions, we could store prefetch assets in a vector only to make sure they stay
            //        cached, and let the event queue handle the prefetching. since the assets are still loaded and owned by our vector,
            //        the prefetch in the event queue will only take as long as it takes to retrieve an asset from the cache

            ActionLoadVisitor visitor(playerActor, *sequence, *actorObject);

            for(auto &action : dbActor.getActions())
            {
                float actionEndTime = std::visit(visitor, action);
                if(actionEndTime > mSequenceDuration)
                {
                    mSequenceDuration = actionEndTime;
                }
            }

            // sorting is unnecessary. the splitting operation does not affect sortedness, and the database guarantees that actions are sorted
            //auto tfPred = [](auto &left, auto &right){ return left.timeOffset < right.timeOffset; };
            //std::sort(playerActor.transformActions.begin(), playerActor.transformActions.end(), tfPred);
            //auto nonTfPred = [](odDb::PlayerActionVariant &left, odDb::PlayerActionVariant &right){ return odDb::Action::getTimeFromVariant(left) < odDb::Action::getTimeFromVariant(right); };
            //std::sort(playerActor.nonTransformActions.begin(), playerActor.nonTransformActions.end(), nonTfPred);
        }

    }

    void SequencePlayer::play(od::LevelObject *playerObject)
    {
        if(mSequence == nullptr)
        {
            return;
        }

        // we fill the object pointers in the actors at the beginning of playback
        //  and clear them at the end as to avoid permanent reference cycles if
        //  an actor owns the sequence player.
        for(auto &actor : mActors)
        {
            actor.second.actorObject = mLevel.getLevelObjectById(actor.second.actorObjectId);

            if(playerObject != nullptr && playerObject == actor.second.actorObject.get())
            {
                Logger::warn() << "Player object is also an actor. This could result in memory leaks if playback never stops";
            }
        }

        if(mSequence->getRunStateModifyStyle() == odDb::ModifyRunStateStyle::STOP_ACTORS)
        {
            for(auto &actor : mActors)
            {
                if(playerObject == actor.second.actorObject.get()) continue;

                actor.second.actorObject->setRunning(false);
            }

        }else if(mSequence->getRunStateModifyStyle() == odDb::ModifyRunStateStyle::STOP_NON_ACTORS)
        {
            mLevel.forEachObject([this, playerObject](od::LevelObject &obj){

                if(playerObject == &obj) return;

                auto it = mActors.find(obj.getObjectId());
                if(it == mActors.end())
                {
                    // not an actor. stop object
                    obj.setRunning(false);
                }

            });

        }else if(mSequence->getRunStateModifyStyle() == odDb::ModifyRunStateStyle::STOP_ALL_OBJECTS)
        {
            mLevel.forEachObject([playerObject](od::LevelObject &obj){

                if(playerObject == &obj) return;

                obj.setRunning(false);

            });
        }

        mPlaying = true;
    }

    void SequencePlayer::stop()
    {
        if(mSequence == nullptr)
        {
            return;
        }

        mPlaying = false;

        // restore previous accumulator if we created our own
        for(auto &pActor : mActors)
        {
            auto &actor = pActor.second;
            auto animPlayer = actor.actorObject->getSkeletonAnimationPlayer();
            if(actor.motionToPositionRootAccumulator != nullptr && animPlayer != nullptr)
            {
                animPlayer->setBoneAccumulator(actor.prevRootAccumulator, 0);
                animPlayer->setBoneModes(actor.prevRootBoneModes, 0);
            }
        }

        // restart objects based on what we did in play()
        if(mSequence->getRunStateModifyStyle() == odDb::ModifyRunStateStyle::STOP_ACTORS)
        {
            for(auto &actor : mActors)
            {
                actor.second.actorObject->setRunning(true);
            }

        }else if(mSequence->getRunStateModifyStyle() == odDb::ModifyRunStateStyle::STOP_NON_ACTORS)
        {
            mLevel.forEachObject([this](od::LevelObject &obj){
                auto it = mActors.find(obj.getObjectId());
                if(it == mActors.end())
                {
                    obj.setRunning(true);
                }
            });

        }else if(mSequence->getRunStateModifyStyle() == odDb::ModifyRunStateStyle::STOP_ALL_OBJECTS)
        {
            mLevel.forEachObject([](od::LevelObject &obj){
                obj.setRunning(true);
            });
        }

        // as to prevent reference cycles. see comment in play()
        for(auto &actor : mActors)
        {
            actor.second.actorObject = nullptr;
        }
    }


    class MotionToPositionAccumulator final : public odAnim::BoneAccumulator
    {
    public:

        MotionToPositionAccumulator(od::LevelObject &obj)
        : mObject(obj)
        {
        }

        virtual void moveRelative(const glm::vec3 &relTranslation, float relTime) override
        {
            mObject.setPosition(mObject.getPosition() + mObject.getRotation()*relTranslation);
        }


    private:

        od::LevelObject &mObject; // FIXME: we should remove this accumulator on the object's destructor

    };


    class NonTransformApplyVisitor
    {
    public:

        NonTransformApplyVisitor(SequencePlayer::Actor &actor, float sequenceTime)
        : mActor(actor)
        , mSequenceTime(sequenceTime)
        {
        }

        void operator()(const odDb::Action &a)
        {
            // we have accounted for every action except transforms, which are
            //  filtered during loading. thus, this should never be reached.
            OD_UNREACHABLE();
        }

        void operator()(const PlayerActionStartAnim &a)
        {
            auto boneModes = a.getRootNodeTranslationModes();

            AnimModes animModes;
            animModes.playbackType = a.animation->isLooping() ? PlaybackType::LOOPING : PlaybackType::NORMAL;
            animModes.boneModes = boneModes;
            animModes.channel = a.channelIndex;
            animModes.speed = a.speed;
            animModes.startTime = a.beginPlayAt;
            animModes.transitionTime = a.transitionTime;

            mActor.actorObject->playAnimation(a.animation, animModes);

            // create accumulator on demand
            bool needsAccumulator = std::any_of(boneModes.begin(), boneModes.end(), [](auto mode){ return mode == BoneMode::ACCUMULATE; });
            auto animPlayer = mActor.actorObject->getSkeletonAnimationPlayer();
            if(needsAccumulator && mActor.motionToPositionRootAccumulator == nullptr && animPlayer != nullptr)
            {
                mActor.prevRootAccumulator = animPlayer->getBoneAccumulator(0);
                mActor.prevRootBoneModes = animPlayer->getBoneModes(0);

                mActor.motionToPositionRootAccumulator = std::make_shared<MotionToPositionAccumulator>(*mActor.actorObject);
                animPlayer->setBoneAccumulator(mActor.motionToPositionRootAccumulator, 0);
            }

            if(animPlayer != nullptr)
            {
                float dt = mSequenceTime - a.timeOffset;
                animPlayer->update(dt);
            }
        }

        void operator()(const PlayerActionPlaySound &a)
        {
            float dt = mSequenceTime - a.timeOffset;
            if(dt > a.duration)
            {
                return;
            }

            // TODO: play sound
        }

        void operator()(const odDb::ActionAttach &a)
        {
        }

        void operator()(const odDb::ActionRunStopAi &a)
        {
            mActor.actorObject->setRunning(a.enableAi);
        }

        void operator()(const odDb::ActionShowHide &a)
        {
            mActor.actorObject->setVisible(a.visible);
        }

        void operator()(const odDb::ActionMessage &a)
        {
            od::Message message = getMessageForCode(a.messageCode);
            mActor.actorObject->receiveMessage(*mActor.actorObject, message);
        }

        void operator()(const odDb::ActionMusic &a)
        {
            //float dt = mSequenceTime - a.timeOffset;
        }


    private:

        SequencePlayer::Actor &mActor;
        float mSequenceTime;

    };

    bool SequencePlayer::update(float relTime)
    {
        if(!mPlaying)
        {
            return false;
        }

        float lastTime = mLastUpdateSequenceTime;
        mSequenceTime += relTime;
        mLastUpdateSequenceTime = mSequenceTime;

        bool sequenceRunning = false;

        // transform actions
        for(auto &actorIt : mActors)
        {
            auto &actor = actorIt.second;

            if(!actor.transformActions.empty())
            {
                auto tfPred = [](float t, odDb::ActionTransform &action){ return t < action.timeOffset; };
                auto right = std::upper_bound(actor.transformActions.begin(), actor.transformActions.end(), mSequenceTime, tfPred);

                if(right != actor.transformActions.begin())
                {
                    // there is a keyframe left of us
                    auto left = right-1;
                    if(right == actor.transformActions.end())
                    {
                        // no keyframe right of us -> apply left as it is, marked as jump
                        _applySingleKeyframe(actor, *left);

                    }else
                    {
                        // there is a keyframe both left and right of us. we can interpolate!
                        _applyInterpolatedKeyframes(actor, *left, *right);
                        sequenceRunning = true;
                    }
                } // else: no keyframe left of us, so no transforms have happened yet.
            }

            if(!actor.nonTransformActions.empty())
            {
                // for non-transforms, we can't interpolate anything. here we simply search all events that happened since last time and apply them.
                //  any missed time can be added by them as they see fit
                NonTransformApplyVisitor visitor(actor, mSequenceTime);
                auto nonTfPredUpper = [](float t, PlayerActionVariant &action){ return t < odDb::Action::getTimeFromVariant(action); };
                auto nonTfPredLower = [](PlayerActionVariant &action, float t){ return odDb::Action::getTimeFromVariant(action) < t; };
                auto begin = std::lower_bound(actor.nonTransformActions.begin(), actor.nonTransformActions.end(), lastTime, nonTfPredLower);
                if(begin != actor.nonTransformActions.end())
                {
                    // there are still events past the last processed point in time
                    auto end = std::upper_bound(begin, actor.nonTransformActions.end(), mSequenceTime, nonTfPredUpper);
                    for(auto it = begin; it < end; ++it)
                    {
                        std::visit(visitor, *it);
                    }

                    if(end != actor.nonTransformActions.end())
                    {
                        sequenceRunning = true;
                    }
                }
            }
        }

        if(!sequenceRunning)
        {
            stop();
        }

        return sequenceRunning;
    }

    void SequencePlayer::skipSequence()
    {
        if(!mPlaying || mSequence == nullptr)
        {
            return;
        }

        mSequenceTime = mSequenceDuration;
    }

    void SequencePlayer::_applySingleKeyframe(Actor &actor, const odDb::ActionTransform &kf)
    {
        if(actor.lastAppliedKeyframe == &kf)
        {
            return;
        }

        od::ObjectStates states;

        if(!kf.ignorePosition())
        {
            states.position = kf.position;
            states.position.setJump(true);
        }

        if(!kf.ignoreRotation())
        {
            states.rotation = kf.rotation;
            states.rotation.setJump(true);
        }

        actor.actorObject->setStates(states);

        actor.lastAppliedKeyframe = &kf;
    }

    void SequencePlayer::_applyInterpolatedKeyframes(Actor &actor, const odDb::ActionTransform &left, const odDb::ActionTransform &right)
    {
        if(left.getRelativeTo() != odDb::ActionTransform::RelativeTo::WORLD || right.getRelativeTo() != odDb::ActionTransform::RelativeTo::WORLD)
        {
            OD_UNIMPLEMENTED();
        }

        // it seems to me like interpolation styles always affect the curve *left* of the keyframe
        if(right.getInterpolationType() == odDb::ActionTransform::InterpolationType::NONE)
        {
            _applySingleKeyframe(actor, left);

        }else
        {
            // TODO: heed other interpolation styles as well (once we figure them out)
            float delta = (mSequenceTime - left.timeOffset) / (right.timeOffset - left.timeOffset);

            od::ObjectStates states;

            if(!left.ignorePosition() && right.ignorePosition())
            {
                states.position = left.position;

            }else if(left.ignorePosition() && !right.ignorePosition())
            {
                states.position = right.position;

            }else if(!left.ignorePosition() && !right.ignorePosition())
            {
                float delta = (mSequenceTime - left.timeOffset) / (right.timeOffset - left.timeOffset);
                auto position = glm::mix(left.position, right.position, delta);
                states.position = position;
            }

            if(!left.ignoreRotation() && right.ignoreRotation())
            {
                states.rotation = left.rotation;

            }else if(left.ignoreRotation() && !right.ignoreRotation())
            {
                states.rotation = right.rotation;

            }else if(!left.ignoreRotation() && !right.ignoreRotation())
            {
                auto rotation = glm::slerp(left.rotation, right.rotation, delta);
                states.rotation = rotation;
            }

            actor.actorObject->setStates(states);
        }

    }

}
