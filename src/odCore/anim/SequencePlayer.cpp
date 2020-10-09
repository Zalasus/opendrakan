
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

        NonTransformApplyVisitor &operator()(const odDb::Action &a)
        {
            OD_UNREACHABLE();
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionStartAnim &a)
        {
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionPlaySound &a)
        {
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionAttach &a)
        {
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionRunStopAi &a)
        {
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionShowHide &a)
        {
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionMessage &a)
        {
            return *this;
        }

        NonTransformApplyVisitor &operator()(const odDb::ActionMusic &a)
        {
            return *this;
        }

    };

    void SequencePlayer::update(float relTime)
    {
        float lastTime = mSequenceTime;
        mSequenceTime += relTime;

        // transform actions
        for(auto &actor : mActors)
        {
            if(!actor.transformActions.empty())
            {
                auto tfPred = [](float t, odDb::ActionTransform &action){ return t < action.timeOffset; };
                auto right = std::upper_bound(actor.transformActions.begin(), actor.transformActions.end(), mSequenceTime, tfPred);
                auto left = right-1;

                if(right == actor.transformActions.end())
                {
                    actor.actorObject.setPosition(left->position);
                    actor.actorObject.setRotation(left->rotation);

                }else
                {
                    // TODO: heed interpolation style flag
                    float delta = (mSequenceTime - left->timeOffset) / (right->timeOffset - left->timeOffset);
                    auto position = glm::mix(left->position, right->position, delta);
                    auto rotation = glm::slerp(left->rotation, right->rotation, delta);

                    actor.actorObject.setPosition(left->position);
                    actor.actorObject.setRotation(left->rotation);
                }
            }

            if(!actor.nonTransformActions.empty())
            {
                NonTransformApplyVisitor visitor;
                auto nonTfPred = [](float t, odDb::ActionVariant &action){ return t < action.as<odDb::Action>()->timeOffset; };
                auto begin = std::upper_bound(actor.nonTransformActions.begin(), actor.nonTransformActions.end(), lastTime, nonTfPred);
                if(begin != actor.nonTransformActions.end())
                {
                    auto end = std::upper_bound(begin+1, actor.nonTransformActions.end(), mSequenceTime, nonTfPred)-1;
                    for(auto it = begin; it < end; ++it)
                    {
                        it->visit(visitor);
                    }
                }
            }
        }
    }

}
