
#include <odCore/state/StateManager.h>

#include <algorithm>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/net/ClientConnector.h>

namespace odState
{

    static const TickNumber TICK_CAPACITY = 16;


    /**
     * @brief An RAII object that disables state updates on the StateManager as long as it lives.
     */
    class ApplyGuard
    {
    public:

        ApplyGuard(StateManager &sm)
        : mStateManager(sm)
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
    };


    StateManager::StateManager(od::Level &level)
    : mLevel(level)
    , mIgnoreStateUpdates(false)
    {
    }

    void StateManager::objectTransformed(od::LevelObject &object, const od::ObjectTransform &tf)
    {
        if(mIgnoreStateUpdates) return;

        auto &objectChanges = mCurrentUpdateChangeMap[object.getObjectId()];
        objectChanges.setTransform(tf);
    }

    void StateManager::objectVisibilityChanged(od::LevelObject &object, bool visible)
    {
        if(mIgnoreStateUpdates) return;

        auto &objectChanges = mCurrentUpdateChangeMap[object.getObjectId()];
        objectChanges.setVisibility(visible);
    }

    void StateManager::objectCustomStateChanged(od::LevelObject &object)
    {
        OD_UNIMPLEMENTED();
    }

    void StateManager::incomingObjectTransformed(TickNumber tick, od::LevelObject &object, const od::ObjectTransform &tf)
    {
        if(mIgnoreStateUpdates) return;
        auto snapshot = _getSnapshot(tick, mIncomingSnapshots);
        auto &objectChanges = snapshot->changesSinceLastSnapshot[object.getObjectId()];

        if(!objectChanges.transformed()) snapshot->discreteChangeCount++;
        objectChanges.setTransform(tf);

        _commitIncomingIfComplete(tick, snapshot);
    }

    void StateManager::incomingObjectVisibilityChanged(TickNumber tick, od::LevelObject &object, bool visible)
    {
        if(mIgnoreStateUpdates) return;
        auto snapshot = _getSnapshot(tick, mIncomingSnapshots);
        auto &objectChanges = snapshot->changesSinceLastSnapshot[object.getObjectId()];

        if(!objectChanges.visibilityChanged()) snapshot->discreteChangeCount++;
        objectChanges.setVisibility(visible);

        _commitIncomingIfComplete(tick, snapshot);
    }

    void StateManager::incomingObjectCustomStateChanged(TickNumber tick, od::LevelObject &object)
    {
        OD_UNIMPLEMENTED();
    }

    void StateManager::confirmIncomingSnapshot(TickNumber tick, double time, size_t changeCount)
    {
        auto stagedSnapshot = _getSnapshot(tick, mIncomingSnapshots);
        stagedSnapshot->realtime = time;
        stagedSnapshot->targetDiscreteChangeCount = changeCount;
        stagedSnapshot->confirmed = true;

        _commitIncomingIfComplete(tick, stagedSnapshot);
    }

    void StateManager::commit(double realtime)
    {
        // apply all new changes to the base state map and count total discrete changes
        size_t discreteChangeCount = 0;
        for(auto &stateChange : mCurrentUpdateChangeMap)
        {
            auto &baseStateChange = mCurrentUpdateChangeMap[stateChange.first];
            baseStateChange.merge(stateChange.second);

            discreteChangeCount += stateChange.second.getDiscreteChangeCount();
        }

        std::lock_guard<std::mutex> lock(mSnapshotMutex);

        if(mSnapshots.size() >= TICK_CAPACITY)
        {
            mSnapshots.pop_front();
        }

        TickNumber nextTick = mSnapshots.empty() ? 0 : mSnapshots.back().tick + 1;
        mSnapshots.emplace_back(nextTick);
        auto &newSnapshot = mSnapshots.back();
        newSnapshot.changesSinceLastSnapshot.swap(mCurrentUpdateChangeMap);
        newSnapshot.realtime = realtime;
        newSnapshot.discreteChangeCount = discreteChangeCount;
    }

    void StateManager::apply(double realtime)
    {
        std::lock_guard<std::mutex> lock(mSnapshotMutex);
        ApplyGuard applyGuard(*this);

        auto pred = [](Snapshot &snapshot, double realtime) { return snapshot.realtime < realtime; };
        auto it = std::lower_bound(mSnapshots.begin(), mSnapshots.end(), realtime, pred);
    }

    void StateManager::apply(TickNumber tick, float lerp)
    {
        OD_UNIMPLEMENTED();
    }

    void StateManager::sendLatestSnapshotToClient(odNet::ClientConnector &c)
    {
        if(mSnapshots.empty())
        {
            throw od::Exception("No snapshot available for sending");
        }

        auto &snapshot = mSnapshots.back();

        for(auto &stateChange : snapshot.changesSinceLastSnapshot)
        {
            if(stateChange.second.transformed())
            {
                c.objectTransformed(snapshot.tick, stateChange.first, stateChange.second.getTransform());
            }

            if(stateChange.second.visibilityChanged())
            {
                c.objectVisibilityChanged(snapshot.tick, stateChange.first, stateChange.second.getVisibility());
            }

            //if(stateTransition.second.animationFrame) c.objectTransformed(tick, stateTransition.first, stateTransition.second.transform);
        }

        c.confirmSnapshot(snapshot.tick, snapshot.realtime, snapshot.discreteChangeCount);
    }

    StateManager::SnapshotIterator StateManager::_getSnapshot(TickNumber tick, std::deque<Snapshot> &snapshots)
    {
        auto pred = [](Snapshot &snapshot, TickNumber tick) { return snapshot.tick == tick; };
        auto it = std::lower_bound(snapshots.begin(), snapshots.end(), tick, pred);
        if(it == snapshots.end())
        {
            return snapshots.emplace(snapshots.begin(), tick);

        }else if(it->tick == tick)
        {
            return it;

        }else
        {
            return snapshots.emplace(it, tick);
        }
    }

    void StateManager::_commitIncomingIfComplete(TickNumber tick, SnapshotIterator incomingSnapshot)
    {
        if(incomingSnapshot->confirmed && (incomingSnapshot->discreteChangeCount == incomingSnapshot->targetDiscreteChangeCount))
        {
            std::lock_guard<std::mutex> lock(mSnapshotMutex);

            if(mSnapshots.size() >= TICK_CAPACITY)
            {
                mSnapshots.pop_front();
            }

            auto snapshot = _getSnapshot(tick, mSnapshots);
            if(snapshot->confirmed) throw od::Exception("Re-committing snapshot");
            *snapshot = std::move(*incomingSnapshot);
            mIncomingSnapshots.erase(incomingSnapshot);
        }
    }

}
