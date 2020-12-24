
#include <odCore/anim/SequencePlayer.h>

#include <algorithm>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Message.h>

#include <odCore/anim/SkeletonAnimationPlayer.h>

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
    {
    }

    class ActionLoadVisitor
    {
    public:

        SequencePlayer::Actor &actor;
        odDb::Sequence &sequence;

        ActionLoadVisitor(SequencePlayer::Actor &a, odDb::Sequence &s)
        : actor(a)
        , sequence(s)
        {
        }

        void operator()(const odDb::ActionTransform &a)
        {
            actor.transformActions.emplace_back(a);
        }

        void operator()(const odDb::ActionStartAnim &a)
        {
            // for some reason, animation refs are stored relative to the object's model's DB, not the sequence DB
            if(actor.actorObject.getModel() == nullptr)
            {
                Logger::error() << "Animation on object without loaded model. Can't load animation asset";
                return;
            }
            auto animDepTable = actor.actorObject.getModel()->getDependencyTable();

            PlayerActionStartAnim newAction(a);
            newAction.animation = animDepTable->loadAsset<odDb::Animation>(a.animationRef);
            if(newAction.animation == nullptr)
            {
                Logger::error() << "Missing animation in sequence: " << a.animationRef;
                return;
            }

            actor.nonTransformActions.emplace_back(newAction);
        }

        void operator()(const odDb::ActionPlaySound &a)
        {
            PlayerActionPlaySound newAction(a);
            newAction.sound = sequence.getDependencyTable()->loadAsset<odDb::Sound>(a.soundRef);
            if(newAction.sound == nullptr)
            {
                Logger::error() << "Missing sound in sequence: " << a.soundRef;
                return;
            }

            actor.nonTransformActions.emplace_back(newAction);
        }

        template <typename _OtherAction>
        void operator()(const _OtherAction &a)
        {
            actor.nonTransformActions.emplace_back(a);
        }
    };

    void SequencePlayer::loadSequence(std::shared_ptr<odDb::Sequence> sequence)
    {
        OD_CHECK_ARG_NONNULL(sequence);

        mSequence = sequence;
        mSequenceTime = 0.0;

        auto &actors = sequence->getActors();
        mActors.clear();
        mActors.reserve(actors.size());
        for(auto &dbActor : actors)
        {
            od::LevelObject *actorObject = mLevel.getLevelObjectById(dbActor.getLevelObjectId());
            if(actorObject == nullptr)
            {
                Logger::warn() << "Actor '" << dbActor.getName() << "' in sequence '" << sequence->getName() << "' has invalid object reference";
                continue;
            }

            auto newActorIt = mActors.insert(std::make_pair(actorObject->getObjectId(), Actor(*actorObject)));
            auto &playerActor = (newActorIt.first)->second;

            // we need to split off transform and non-transform actions into their own vectors, as to make interpolation easier.
            //  we take this opportunity to prefetch animations and sounds, which are represented by alternative action variants.

            ActionLoadVisitor visitor(playerActor, *sequence);

            for(auto &action : dbActor.getActions())
            {
                std::visit(visitor, action);
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
        if(mSequence->getRunStateModifyStyle() == odDb::ModifyRunStateStyle::STOP_ACTORS)
        {
            for(auto &actor : mActors)
            {
                if(playerObject != nullptr && actor.second.actorObject.getObjectId() == playerObject->getObjectId()) continue;

                actor.second.actorObject.setRunning(false);
            }

        }else if(mSequence->getRunStateModifyStyle() == odDb::ModifyRunStateStyle::STOP_NON_ACTORS)
        {
            mLevel.forEachObject([this, playerObject](od::LevelObject &obj){

                if(playerObject != nullptr && obj.getObjectId() == playerObject->getObjectId()) return;

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

                if(playerObject != nullptr && obj.getObjectId() == playerObject->getObjectId()) return;

                obj.setRunning(false);

            });
        }
    }

    class NonTransformApplyVisitor
    {
    public:

        NonTransformApplyVisitor(od::LevelObject &obj, float sequenceTime)
        : mObject(obj)
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
            float dt = mSequenceTime - a.timeOffset;

            auto animPlayer = mObject.getSkeletonAnimationPlayer();
            if(animPlayer != nullptr && a.animation != nullptr)
            {
                animPlayer->playAnimation(a.animation, odAnim::PlaybackType::NORMAL, a.speed);
                animPlayer->update(dt); // FIXME: is this really that great of an idea? unless we send this over the network, this might cause glitches
            }

        }

        void operator()(const PlayerActionPlaySound &a)
        {
            //float dt = mSequenceTime - a.timeOffset;
        }

        void operator()(const odDb::ActionAttach &a)
        {
            //float dt = mSequenceTime - a.timeOffset;
        }

        void operator()(const odDb::ActionRunStopAi &a)
        {
            mObject.setRunning(a.enableAi);
        }

        void operator()(const odDb::ActionShowHide &a)
        {
            mObject.setVisible(a.visible);
        }

        void operator()(const odDb::ActionMessage &a)
        {
            od::Message message = getMessageForCode(a.messageCode);
            mObject.receiveMessage(mObject, message);
        }

        void operator()(const odDb::ActionMusic &a)
        {
            //float dt = mSequenceTime - a.timeOffset;
        }


    private:

        od::LevelObject &mObject;
        float mSequenceTime;

    };

    bool SequencePlayer::update(float relTime)
    {
        float lastTime = mSequenceTime;
        mSequenceTime += relTime;

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
                        _applySingleKeyframe(actor.actorObject, *left);

                    }else
                    {
                        // there is a keyframe both left and right of us. we can interpolate!
                        _applyInterpolatedKeyframes(actor.actorObject, *left, *right);
                        sequenceRunning = true;
                    }
                } // else: no keyframe left of us, so no transforms have happened yet.
            }

            if(!actor.nonTransformActions.empty())
            {
                // for non-transforms, we can't interpolate anything. here we simply search all events that happened since last time and apply them.
                //  any missed time can be added by them as they see fit
                NonTransformApplyVisitor visitor(actor.actorObject, mSequenceTime);
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

        return sequenceRunning;
    }

    void SequencePlayer::_applySingleKeyframe(od::LevelObject &obj, const odDb::ActionTransform &kf)
    {
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

        obj.setStates(states);
    }

    void SequencePlayer::_applyInterpolatedKeyframes(od::LevelObject &obj, const odDb::ActionTransform &left, const odDb::ActionTransform &right)
    {
        if(left.getRelativeTo() != odDb::ActionTransform::RelativeTo::WORLD || right.getRelativeTo() != odDb::ActionTransform::RelativeTo::WORLD)
        {
            OD_UNIMPLEMENTED();
        }

        // it seems to me like interpolation styles always affect the curve *left* of the keyframe
        if(right.getInterpolationType() == odDb::ActionTransform::InterpolationType::NONE)
        {
            _applySingleKeyframe(obj, left);

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

            obj.setStates(states);
        }

    }

}
