
#include <odCore/anim/SequencePlayer.h>

#include <algorithm>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

namespace odAnim
{

    SequencePlayer::SequencePlayer()
    : mSequenceTime(0.0)
    {
    }

    void SequencePlayer::setSequence(od::Level &level, odDb::Sequence &s)
    {
        mSequenceTime = 0.0;

        auto &actors = s.getActors();
        mActors.clear();
        mActors.reserve(actors.size());
        for(auto &dbActor : actors)
        {
            od::LevelObject *actorObject = level.getLevelObjectById(dbActor.getLevelObjectId());
            if(actorObject == nullptr)
            {
                Logger::warn() << "Actor '" << dbActor.getName() << "' in sequence '" << s.getName() << "' has invalid object reference";
                continue;
            }

            mActors.emplace_back(*actorObject);
            auto &playerActor = mActors.back();

            for(auto &action : dbActor.getActions())
            {
                const odDb::ActionTransform *tf = action.as<odDb::ActionTransform>();
                if(tf != nullptr)
                {
                    playerActor.transformActions.push_back(*tf);

                }else
                {
                    playerActor.nonTransformActions.push_back(action);
                }
            }

            auto tfPred = [](auto &left, auto &right){ return left.timeOffset < right.timeOffset; };
            std::sort(playerActor.transformActions.begin(), playerActor.transformActions.end(), tfPred);

            auto nonTfPred = [](odDb::ActionVariant &left, odDb::ActionVariant &right){ return left.as<odDb::Action>()->timeOffset < right.as<odDb::Action>()->timeOffset; };
            std::sort(playerActor.nonTransformActions.begin(), playerActor.nonTransformActions.end(), nonTfPred);
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

        NonTransformApplyVisitor &operator()(const odDb::Action &a)
        {
            // we have accounted for every action except transforms, which are
            //  filtered during loading. thus, this should never be reached.
            OD_UNREACHABLE();
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionStartAnim &a)
        {
            float dt = mSequenceTime - a.timeOffset;
            Logger::info() << "start anim. dt=" << dt;
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionPlaySound &a)
        {
            float dt = mSequenceTime - a.timeOffset;
            Logger::info() << "play sound. dt=" << dt;
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionAttach &a)
        {
            float dt = mSequenceTime - a.timeOffset;
            Logger::info() << "attach. dt=" << dt;
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionRunStopAi &a)
        {
            float dt = mSequenceTime - a.timeOffset;
            Logger::info() << "run/stop. dt=" << dt;
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionShowHide &a)
        {
            float dt = mSequenceTime - a.timeOffset;
            Logger::info() << "show/hide. dt=" << dt;
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionMessage &a)
        {
            float dt = mSequenceTime - a.timeOffset;
            Logger::info() << "message: " << a.messageCode << " dt=" << dt;
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionMusic &a)
        {
            float dt = mSequenceTime - a.timeOffset;
            Logger::info() << "music. dt=" << dt;
            return *this;
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
        for(auto &actor : mActors)
        {
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
                        // no keyframe right of us -> apply left as it is
                        actor.actorObject.setPosition(left->position);
                        actor.actorObject.setRotation(left->rotation);

                    }else
                    {
                        // there is a keyframe both left and right of us. we can interpolate!
                        // TODO: heed interpolation style flag
                        float delta = (mSequenceTime - left->timeOffset) / (right->timeOffset - left->timeOffset);
                        auto position = glm::mix(left->position, right->position, delta);
                        auto rotation = glm::slerp(left->rotation, right->rotation, delta);

                        actor.actorObject.setPosition(position);
                        actor.actorObject.setRotation(rotation);

                        sequenceRunning = true;
                    }
                } // else: no keyframe left of us, so no transforms have happened yet.
            }

            if(!actor.nonTransformActions.empty())
            {
                // for non-transforms, we can't interpolate anything. here we simply search all events that happened since last time and apply them.
                //  any missed time can be added by them as they see fit
                NonTransformApplyVisitor visitor(actor.actorObject, mSequenceTime);
                auto nonTfPredUpper = [](float t, odDb::ActionVariant &action){ return t < action.as<odDb::Action>()->timeOffset; };
                auto nonTfPredLower = [](odDb::ActionVariant &action, float t){ return action.as<odDb::Action>()->timeOffset < t; };
                auto begin = std::lower_bound(actor.nonTransformActions.begin(), actor.nonTransformActions.end(), lastTime, nonTfPredLower);
                if(begin != actor.nonTransformActions.end())
                {
                    // there are still events past the last processed point in time
                    auto end = std::upper_bound(begin, actor.nonTransformActions.end(), mSequenceTime, nonTfPredUpper);
                    for(auto it = begin; it < end; ++it)
                    {
                        it->visit(visitor);
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

}
