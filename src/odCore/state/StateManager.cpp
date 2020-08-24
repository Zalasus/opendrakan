
#include <odCore/state/StateManager.h>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/net/ClientConnector.h>

namespace odState
{

    static const TickNumber BACKLOG_TICKS = 16;
    static const TickNumber FUTURE_TICKS = 3; // this includes the tick currently built, so it should at least be 1 TODO: on server, this *can* be 1. make dynamic


    StateManager::RollbackGuard::RollbackGuard(StateManager &sm)
    : mStateManager(&sm)
    {
        mStateManager->mIgnoreStateUpdates = true;
    }

    StateManager::RollbackGuard::RollbackGuard(RollbackGuard &&g)
    : mStateManager(g.mStateManager)
    {
        g.mStateManager = nullptr;
    }

    StateManager::RollbackGuard::~RollbackGuard()
    {
        if(mStateManager != nullptr)
        {
            mStateManager->mIgnoreStateUpdates = false;

            // TODO: reverse rollback here
        }
    }


    StateManager::StateManager(od::Level &level)
    : mLevel(level)
    , mIgnoreStateUpdates(false)
    , mOldestTickIndex(0)
    , mOldestTick(0)
    , mEvents(0)
    {
        mStateTransitions.resize(BACKLOG_TICKS + FUTURE_TICKS);
    }

    TickNumber StateManager::getMaxTick() const
    {
        return mOldestTick + mStateTransitions.size() - 1;
    }

    TickNumber StateManager::getCurrentTick() const
    {
        return mOldestTick + BACKLOG_TICKS;
    }

    void StateManager::objectTransformed(od::LevelObject &object, const od::ObjectTransform &tf, TickNumber tick)
    {
        if(mIgnoreStateUpdates) return;

        auto &transitionMap = _getTransitionMapForTick(tick);
        auto &objEvent = transitionMap[object.getObjectId()];
        objEvent.transformed = true;
        objEvent.transform = tf;
    }

    void StateManager::objectVisibilityChanged(od::LevelObject &object, bool visible, TickNumber tick)
    {
        if(mIgnoreStateUpdates) return;

        auto &transitionMap = _getTransitionMapForTick(tick);
        auto &objEvent = transitionMap[object.getObjectId()];
        objEvent.visibilityChanged = true;
        objEvent.visibility = visible;
    }

    StateManager::RollbackGuard StateManager::rollback(TickNumber tick)
    {
        RollbackGuard guard(*this);

        // TODO: implement

        return std::move(guard);
    }

    void StateManager::commit()
    {
        // apply all new changes to the base state map
        for(auto &stateTransition : _getCurrentTransitionMap())
        {
            auto &baseStateTransition = mBaseStateTransitionMap[stateTransition.first];
            baseStateTransition.merge(stateTransition.second);
        }

        mOldestTickIndex = (mOldestTickIndex + 1) % mStateTransitions.size();
        ++mOldestTick;

        // since the currentTransitionMap has now wrapped around to an old one, clear that so the new tick starts fresh
        _getCurrentTransitionMap().clear();
    }

    void StateManager::sendToClient(TickNumber tick, odNet::ClientConnector &c)
    {
        for(auto &stateTransition : _getTransitionMapForTick(tick))
        {
            if(stateTransition.second.transformed) c.objectTransformed(tick, stateTransition.first, stateTransition.second.transform);
            if(stateTransition.second.visibilityChanged) c.objectVisibilityChanged(tick, stateTransition.first, stateTransition.second.visibility);
            //if(stateTransition.second.animationFrame) c.objectTransformed(tick, stateTransition.first, stateTransition.second.transform);
        }
    }

    StateManager::StateTransitionMap &StateManager::_getTransitionMapForTick(TickNumber tick)
    {
        auto offset = tick - mOldestTick;
        if(offset < 0 || offset >= static_cast<decltype(offset)>(mStateTransitions.size()))
        {
            throw od::Exception("Invalid tick number");
        }

        auto baseOffset = (offset + mOldestTickIndex) % mStateTransitions.size();

        return mStateTransitions[baseOffset];
    }

    StateManager::StateTransitionMap &StateManager::_getCurrentTransitionMap()
    {
        auto baseOffset = (BACKLOG_TICKS + mOldestTickIndex) % mStateTransitions.size();
        return mStateTransitions[baseOffset];
    }

}
