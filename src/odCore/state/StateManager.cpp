
#include <odCore/state/StateManager.h>

#include <algorithm>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/net/DownlinkConnector.h>

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

    TickNumber StateManager::getLatestTick()
    {
        std::lock_guard<std::mutex> lock(mSnapshotMutex);

        return mSnapshots.empty() ? FIRST_TICK - 1 : mSnapshots.back().tick;
    }

    double StateManager::getLatestRealtime()
    {
        std::lock_guard<std::mutex> lock(mSnapshotMutex);

        return mSnapshots.empty() ? 0.0 : mSnapshots.back().realtime;
    }

    void StateManager::objectTranslated(od::LevelObject &object, const glm::vec3 &p)
    {
        if(mIgnoreStateUpdates) return;
        mCurrentUpdateChangeMap[object.getObjectId()].baseStates.position = p;
    }

    void StateManager::objectRotated(od::LevelObject &object, const glm::quat &q)
    {
        if(mIgnoreStateUpdates) return;
        mCurrentUpdateChangeMap[object.getObjectId()].baseStates.rotation = q;
    }

    void StateManager::objectScaled(od::LevelObject &object, const glm::vec3 &s)
    {
        if(mIgnoreStateUpdates) return;
        mCurrentUpdateChangeMap[object.getObjectId()].baseStates.scale = s;
    }

    void StateManager::objectVisibilityChanged(od::LevelObject &object, bool visible)
    {
        if(mIgnoreStateUpdates) return;
        mCurrentUpdateChangeMap[object.getObjectId()].baseStates.visibility = visible;
    }

    void StateManager::objectCustomStateChanged(od::LevelObject &object)
    {
        OD_UNIMPLEMENTED();
    }

    void StateManager::incomingObjectTranslated(TickNumber tick, od::LevelObject &object, const glm::vec3 &p)
    {
        auto snapshot = _getSnapshot(tick, mIncomingSnapshots, true);
        snapshot->changes[object.getObjectId()].baseStates.position = p;
        _commitIncomingIfComplete(tick, snapshot);
    }

    void StateManager::incomingObjectRotated(TickNumber tick, od::LevelObject &object, const glm::quat &r)
    {
        auto snapshot = _getSnapshot(tick, mIncomingSnapshots, true);
        snapshot->changes[object.getObjectId()].baseStates.rotation = r;
        _commitIncomingIfComplete(tick, snapshot);
    }

    void StateManager::incomingObjectScaled(TickNumber tick, od::LevelObject &object, const glm::vec3 &s)
    {
        auto snapshot = _getSnapshot(tick, mIncomingSnapshots, true);
        snapshot->changes[object.getObjectId()].baseStates.scale = s;
        _commitIncomingIfComplete(tick, snapshot);
    }

    void StateManager::incomingObjectVisibilityChanged(TickNumber tick, od::LevelObject &object, bool visible)
    {
        auto snapshot = _getSnapshot(tick, mIncomingSnapshots, true);
        snapshot->changes[object.getObjectId()].baseStates.visibility = visible;
        _commitIncomingIfComplete(tick, snapshot);
    }

    void StateManager::incomingObjectCustomStateChanged(TickNumber tick, od::LevelObject &object)
    {
        OD_UNIMPLEMENTED();
    }

    void StateManager::confirmIncomingSnapshot(TickNumber tick, double time, size_t changeCount)
    {
        auto stagedSnapshot = _getSnapshot(tick, mIncomingSnapshots, true);
        stagedSnapshot->realtime = time;
        stagedSnapshot->targetDiscreteChangeCount = changeCount;
        stagedSnapshot->confirmed = true;

        _commitIncomingIfComplete(tick, stagedSnapshot);
    }

    void StateManager::commit(double realtime)
    {
        std::lock_guard<std::mutex> lock(mSnapshotMutex);

        if(mSnapshots.size() >= TICK_CAPACITY)
        {
            // TODO: reclaim the discarded change map so we don't allocate a new one for every commit
            mSnapshots.pop_front();
        }

        TickNumber nextTick = mSnapshots.empty() ? FIRST_TICK : mSnapshots.back().tick + 1;
        mSnapshots.emplace_back(nextTick);

        auto &newSnapshot = mSnapshots.back();
        newSnapshot.changes = mCurrentUpdateChangeMap;
        newSnapshot.realtime = realtime;
    }

    void StateManager::apply(double realtime)
    {
        std::lock_guard<std::mutex> lock(mSnapshotMutex);
        ApplyGuard applyGuard(*this);

        if(mSnapshots.empty())
        {
            // can't apply anything on an empty timeline. we are done right away.
            return;
        }

        // find the first snapshots with a time later than the requested one
        auto pred = [](double realtime, Snapshot &snapshot) { return realtime < snapshot.realtime; };
        auto it = std::upper_bound(mSnapshots.begin(), mSnapshots.end(), realtime, pred);

        if(it == mSnapshots.end())
        {
            // the latest snapshot is older than the requested time -> extrapolate
            //  TODO: extrapolation not implemented. applying latest snapshot verbatim for now
            for(auto &objChange : mSnapshots.back().changes)
            {
                od::LevelObject *obj = mLevel.getLevelObjectById(objChange.first);
                if(obj == nullptr) continue;

                obj->applyStates(objChange.second.baseStates);
            }

        }else if(it == mSnapshots.begin())
        {
            // we only have one snapshot in the timeline, and it's later than the requested time.
            //  extrapolating here is probably unnecessary, so we just apply the snapshot as if it happened right now.
            for(auto &objChange : it->changes)
            {
                od::LevelObject *obj = mLevel.getLevelObjectById(objChange.first);
                if(obj == nullptr) continue;

                obj->applyStates(objChange.second.baseStates);
            }

        }else
        {
            Snapshot &a = *(it-1);
            Snapshot &b = *it;

            double delta = (realtime - a.realtime)/(b.realtime - a.realtime);

            for(auto &objChange : a.changes)
            {
                od::LevelObject *obj = mLevel.getLevelObjectById(objChange.first);
                if(obj == nullptr) continue;

                auto changeInB = b.changes.find(objChange.first);
                if(changeInB == b.changes.end())
                {
                    // no corresponding change in B. this should not happen, as
                    //  all snapshots reflect all changes since load. for now, assume steady state
                    obj->applyStates(objChange.second.baseStates);

                }else
                {
                    // TODO: a flag indicating that a state has not changed since the last snapshot might improve performance a tiny bit by ommitting the lerp.
                    //  we still have to store full snapshots, as we must move through the timeline arbitrarily, and for every missing state, we'd have to
                    //  search previous and intermediate snapshots to recover the original state
                    od::ObjectStates lerped;
                    lerped.lerp(objChange.second.baseStates, changeInB->second.baseStates, delta);
                    obj->applyStates(lerped);
                }
            }
        }
    }

    void StateManager::sendSnapshotToClient(TickNumber tickToSend, odNet::DownlinkConnector &c, TickNumber lastSentSnapshot)
    {
        std::lock_guard<std::mutex> lock(mSnapshotMutex);

        auto toSend = _getSnapshot(tickToSend, mSnapshots, false);
        if(toSend == mSnapshots.end())
        {
            throw od::Exception("Snapshot with given tick not available for sending");
        }

        size_t discreteChangeCount = 0;

        auto lastSent = _getSnapshot(lastSentSnapshot, mSnapshots, false);

        for(auto &stateChange : toSend->changes)
        {
            od::ObjectStates filteredChange = stateChange.second.baseStates;
            if(lastSent != mSnapshots.end())
            {
                auto prevChange = lastSent->changes.find(stateChange.first);
                if(prevChange != lastSent->changes.end())
                {
                    filteredChange.deltaEncode(filteredChange, prevChange->second.baseStates);
                }
            }

            //ApplyChangeToDownlinkVisitor applyVisitor(tickToSend, c, stateChange.first);
            //filteredChange.visit(applyVisitor);

            discreteChangeCount += filteredChange.countStatesWithValue();
        }

        c.confirmSnapshot(tickToSend, toSend->realtime, discreteChangeCount);
    }

    StateManager::SnapshotIterator StateManager::_getSnapshot(TickNumber tick, std::deque<Snapshot> &snapshots, bool createIfNotFound)
    {
        auto pred = [](Snapshot &snapshot, TickNumber tick) { return snapshot.tick < tick; };
        auto it = std::lower_bound(snapshots.begin(), snapshots.end(), tick, pred);

        if(it == snapshots.end() || it->tick != tick)
        {
            if(createIfNotFound)
            {
                return snapshots.emplace(it, tick);

            }else
            {
                return snapshots.end();
            }

        }else
        {
            return it;
        }
    }

    void StateManager::_commitIncomingIfComplete(TickNumber tick, SnapshotIterator incomingSnapshot)
    {
        if(!incomingSnapshot->confirmed)
        {
            return;
        }

        // even though we might count the total discrete changes more than once here, this
        //  should happen only rarely in case a confirmation packet arrives earlier than a change.
        //  however, doing it this way reduces the amount of dependent states in our program, which is a plus
        size_t discreteChangeCount = 0;
        for(auto &change : incomingSnapshot->changes)
        {
            discreteChangeCount += change.second.baseStates.countStatesWithValue();
        }

        if(incomingSnapshot->targetDiscreteChangeCount != discreteChangeCount)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(mSnapshotMutex);

        if(mSnapshots.size() >= TICK_CAPACITY)
        {
            mSnapshots.pop_front();
        }

        // merge incoming with previous full snapshot
        //  TODO: it might be clever to explicitly state the snapshot that the delta encoding used
        //  FIXME: due to out-of-order transport, an incoming snapshot might get confirmed before the one it's delta encoding depends on. take that into account!!
        if(!mSnapshots.empty())
        {
            auto &baseSnapshot = mSnapshots.back();
            for(auto &baseChange : baseSnapshot.changes)
            {
                auto &deltaChange = incomingSnapshot->changes[baseChange.first];
                deltaChange.baseStates.merge(baseChange.second.baseStates, deltaChange.baseStates);
            }
        }

        auto snapshot = _getSnapshot(tick, mSnapshots, true);
        if(snapshot->confirmed) throw od::Exception("Re-committing snapshot");
        *snapshot = std::move(*incomingSnapshot);
        mIncomingSnapshots.erase(incomingSnapshot);
    }

}
