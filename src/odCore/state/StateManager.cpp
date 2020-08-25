
#include <odCore/state/StateManager.h>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/net/ClientConnector.h>

namespace odState
{

    static const TickNumber TICK_CAPACITY = 16;


    /**
     * @brief An RAII object that disables state updates on the StateManager as long as it lives
     */
    class ApplyGuard
    {
    public:

        ApplyGuard(StateManager &sm)
        : mStateManager(sm)
        , mLock(sm.mUpdateMutex)
        {
            mStateManager.mIgnoreStateUpdates = true;
        }

        ApplyGuard(const ApplyGuard &g) = delete;

        ~ApplyGuard()
        {
            mStateManager.mIgnoreStateUpdates = false;
        }


    private:

        StateManager &mStateManager;
        std::lock_guard<std::mutex> mLock;
    };


    StateManager::CheckoutGuard::CheckoutGuard(StateManager &sm)
    : mStateManager(&sm)
    {
    }

    StateManager::CheckoutGuard::CheckoutGuard(CheckoutGuard &&g)
    : mStateManager(g.mStateManager)
    {
        g.mStateManager = nullptr;
    }

    StateManager::CheckoutGuard::~CheckoutGuard()
    {
        if(mStateManager != nullptr)
        {
            // TODO: reverse checkout here
        }
    }


    StateManager::StateManager(od::Level &level)
    : mLevel(level)
    , mIgnoreStateUpdates(false)
    , mOldestTickIndex(0)
    , mOldestTick(0)
    , mEvents(0)
    {
        mStateTransitions.resize(TICK_CAPACITY);
    }

    TickNumber StateManager::getLatestTick() const
    {
        return mOldestTick + mStateTransitions.size() - 1;
    }

    TickNumber StateManager::getOldestTick() const
    {
        return mOldestTick;
    }

    void StateManager::objectTransformed(od::LevelObject &object, const od::ObjectTransform &tf, TickNumber tick)
    {
        if(mIgnoreStateUpdates) return;

        std::lock_guard<std::mutex> lock(mUpdateMutex);

        auto &transitionMap = _getTransitionMapForTick(tick);
        auto &objEvent = transitionMap[object.getObjectId()];
        objEvent.setTransform(tf);
    }

    void StateManager::objectVisibilityChanged(od::LevelObject &object, bool visible, TickNumber tick)
    {
        if(mIgnoreStateUpdates) return;

        std::lock_guard<std::mutex> lock(mUpdateMutex);

        auto &transitionMap = _getTransitionMapForTick(tick);
        auto &objEvent = transitionMap[object.getObjectId()];
        objEvent.setVisibility(visible);
    }

    void StateManager::objectCustomStateChanged(od::LevelObject &object, TickNumber tick)
    {
    }

    void StateManager::apply(TickNumber tick, float lerp)
    {
        ApplyGuard guard(*this);

        lerp = 0.0;

        if(lerp == 0.0) // TODO: epsilon
        {
            for(auto &stateTransition : _getTransitionMapForTick(tick))
            {
                auto obj = mLevel.getLevelObjectById(stateTransition.first); // TODO: maybe store this pointer in the transition object? would spare us lots of unnecessary lookups.
                if(obj == nullptr) continue;

                if(stateTransition.second.transformed()) obj->transform(stateTransition.second.getTransform());
                if(stateTransition.second.visibilityChanged()) obj->setVisible(stateTransition.second.getVisibility());
                //if(stateTransition.second.animationFrame) c.objectTransformed(tick, stateTransition.first, stateTransition.second.transform);
            }

        }else
        {
            OD_UNIMPLEMENTED();
        }
    }

    StateManager::CheckoutGuard StateManager::checkout(TickNumber tick)
    {
        CheckoutGuard guard(*this);

        // TODO: implement

        return std::move(guard);
    }

    void StateManager::advance()
    {
        std::lock_guard<std::mutex> lock(mUpdateMutex);

        // apply all new changes to the base state map
        for(auto &stateTransition : _getTransitionMapForTick(getLatestTick()))
        {
            auto &baseStateTransition = mBaseStateTransitionMap[stateTransition.first];
            baseStateTransition.merge(stateTransition.second);
        }

        mOldestTickIndex = (mOldestTickIndex + 1) % mStateTransitions.size();
        ++mOldestTick;

        // since the latest tick has now wrapped around to an old one, clear that so it starts fresh
        _getTransitionMapForTick(getLatestTick()).clear();
    }

    void StateManager::advanceUntil(TickNumber tick)
    {
        if(tick < getOldestTick())
        {
            throw od::Exception("Tick is out of reach");
        }
        
        while(getLatestTick() < tick)
        {
            advance();
        }
    }

    void StateManager::sendToClient(TickNumber tick, odNet::ClientConnector &c)
    {
        for(auto &stateTransition : _getTransitionMapForTick(tick))
        {
            if(stateTransition.second.transformed())
            {
                c.objectTransformed(tick, stateTransition.first, stateTransition.second.getTransform());
            }

            if(stateTransition.second.visibilityChanged())
            {
                c.objectVisibilityChanged(tick, stateTransition.first, stateTransition.second.getVisibility());
            }

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

}
