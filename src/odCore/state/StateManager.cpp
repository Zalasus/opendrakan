
#include <odCore/state/StateManager.h>

#include <algorithm>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/UplinkConnector.h>

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
            mStateManager.mDisallowStateUpdates = true;
        }

        ApplyGuard(const ApplyGuard &g) = delete;

        ~ApplyGuard()
        {
            mStateManager.mDisallowStateUpdates = false;
        }


    private:

        StateManager &mStateManager;
    };


    StateManager::StateManager(od::Level &level)
    : mLevel(level)
    , mDisallowStateUpdates(false)
    {
    }

    void StateManager::setUplinkConnector(std::shared_ptr<odNet::UplinkConnector> c)
    {
        mUplinkConnectorForAck = c;
    }

    TickNumber StateManager::getLatestTick()
    {
        return mSnapshots.empty() ? INVALID_TICK : mSnapshots.back().tick;
    }

    double StateManager::getLatestRealtime()
    {
        return mSnapshots.empty() ? 0.0 : mSnapshots.back().realtime;
    }

    void StateManager::objectStatesChanged(od::LevelObject &object, const od::ObjectStates &newStates)
    {
        _throwIfStateUpdatesDisallowed();

        auto &storedStates = mCurrentUpdateObjectStates[object.getObjectId()];
        storedStates.merge(storedStates, newStates);
    }

    void StateManager::objectExtraStatesChanged(od::LevelObject &object, const odState::StateBundleBase &states)
    {
        _throwIfStateUpdatesDisallowed();

        OD_UNIMPLEMENTED();
    }

    void StateManager::incomingObjectStatesChanged(TickNumber tick, od::LevelObjectId objectId, const od::ObjectStates &states)
    {
        auto snapshotIt = _getSnapshot(tick, mIncomingSnapshots, true);
        auto &objectStates = snapshotIt->objectStates[objectId];
        objectStates.merge(objectStates, states);
        _commitIncomingIfComplete(tick, snapshotIt);
    }

    void StateManager::incomingObjectExtraStatesChanged(TickNumber tick, od::LevelObjectId objectId, const odState::StateBundleBase &states)
    {
        OD_UNIMPLEMENTED();
    }

    void StateManager::confirmIncomingSnapshot(TickNumber tick, double time, size_t changeCount, TickNumber referenceTick)
    {
        auto stagedSnapshot = _getSnapshot(tick, mIncomingSnapshots, true); // TODO: clean up incoming snapshots that have never been confirmed
        stagedSnapshot->realtime = time;
        stagedSnapshot->targetDiscreteChangeCount = changeCount;
        stagedSnapshot->confirmed = true;
        stagedSnapshot->referenceSnapshot = referenceTick;

        _commitIncomingIfComplete(tick, stagedSnapshot);
    }

    void StateManager::commit(double realtime)
    {
        TickNumber nextTick = mSnapshots.empty() ? FIRST_TICK : mSnapshots.back().tick + 1;

        if(mSnapshots.size() >= TICK_CAPACITY)
        {
            // snapshot capacity reached. reclaim oldest snapshot so we don't have to reallocate the change map
            auto oldSnapshot = std::move(mSnapshots.front());
            mSnapshots.pop_front();
            oldSnapshot.tick = nextTick;
            oldSnapshot.realtime = realtime;
            oldSnapshot.objectStates = mCurrentUpdateObjectStates;
            oldSnapshot.extraStates = mCurrentUpdateExtraStates;
            mSnapshots.emplace_back(std::move(oldSnapshot));

        }else
        {
            mSnapshots.emplace_back(nextTick);
            auto &newSnapshot = mSnapshots.back();
            newSnapshot.objectStates = mCurrentUpdateObjectStates;
            newSnapshot.extraStates = mCurrentUpdateExtraStates;
            newSnapshot.realtime = realtime;
        }
    }

    void StateManager::apply(double realtime)
    {
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
            for(auto &objState : mSnapshots.back().objectStates)
            {
                od::LevelObject *obj = mLevel.getLevelObjectById(objState.first);
                if(obj == nullptr) continue;

                obj->setStatesUntracked(objState.second);
            }

        }else if(it == mSnapshots.begin())
        {
            // we only have one snapshot in the timeline, and it's later than the requested time.
            //  extrapolating here is probably unnecessary, so we just apply the snapshot as if it happened right now.
            for(auto &objState : it->objectStates)
            {
                od::LevelObject *obj = mLevel.getLevelObjectById(objState.first);
                if(obj == nullptr) continue;

                obj->setStatesUntracked(objState.second);
            }

        }else
        {
            Snapshot &a = *(it-1);
            Snapshot &b = *it;

            double delta = (realtime - a.realtime)/(b.realtime - a.realtime);

            for(auto &objState : a.objectStates)
            {
                od::LevelObject *obj = mLevel.getLevelObjectById(objState.first);
                if(obj == nullptr) continue;

                auto stateInB = b.objectStates.find(objState.first);
                if(stateInB == b.objectStates.end())
                {
                    // no corresponding change in B. this should not happen, as
                    //  all snapshots reflect all changes since load. for now, assume steady state
                    Logger::warn() << "Incomplete timeline. A tracked state seems to have disappeared";
                    obj->setStatesUntracked(objState.second);

                }else
                {
                    // TODO: a flag indicating that a state has not changed since the last snapshot might improve performance a tiny bit by ommitting the lerp.
                    //  we still have to store full snapshots, as we must move through the timeline arbitrarily, and for every missing state, we'd have to
                    //  search previous and intermediate snapshots to recover the original state
                    od::ObjectStates lerped;
                    lerped.lerp(objState.second, stateInB->second, delta);
                    obj->setStatesUntracked(lerped);
                }
            }
        }
    }

    void StateManager::sendSnapshotToClient(TickNumber tickToSend, odNet::DownlinkConnector &c, TickNumber referenceSnapshot)
    {
        auto toSend = _getSnapshot(tickToSend, mSnapshots, false);
        if(toSend == mSnapshots.end())
        {
            throw od::Exception("Snapshot with given tick not available for sending");
        }

        size_t discreteChangeCount = 0;

        auto reference = (referenceSnapshot != INVALID_TICK) ? _getSnapshot(referenceSnapshot, mSnapshots, false) : mSnapshots.end();

        for(auto &objState : toSend->objectStates)
        {
            od::ObjectStates encodedState = objState.second;
            if(reference != mSnapshots.end())
            {
                auto referenceState = reference->objectStates.find(objState.first);
                if(referenceState != reference->objectStates.end())
                {
                    encodedState.deltaEncode(referenceState->second, encodedState);
                }
            }

            size_t changeCount = encodedState.countStatesWithValue();
            if(changeCount > 0)
            {
                c.objectStatesChanged(tickToSend, objState.first, encodedState);
            }

            discreteChangeCount += changeCount;
        }

        c.confirmSnapshot(tickToSend, toSend->realtime, discreteChangeCount, referenceSnapshot);
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
        for(auto &state : incomingSnapshot->objectStates)
        {
            discreteChangeCount += state.second.countStatesWithValue();
        }

        if(incomingSnapshot->targetDiscreteChangeCount == discreteChangeCount)
        {
            // this snapshot is complete! move it to the timeline
            if(mSnapshots.size() >= TICK_CAPACITY)
            {
                mSnapshots.pop_front();
            }

            // undo delta-encoding by merging incoming with the reference snapshot (only if this is not a full snapshot)
            if(incomingSnapshot->referenceSnapshot != INVALID_TICK)
            {
                auto referenceSnapshot = _getSnapshot(incomingSnapshot->referenceSnapshot, mSnapshots, false);
                if(referenceSnapshot == mSnapshots.end())
                {
                    throw od::Exception("Reference snapshot no longer contained in timeline");
                }

                for(auto &referenceState : referenceSnapshot->objectStates)
                {
                    auto &deltaState = incomingSnapshot->objectStates[referenceState.first];
                    deltaState.merge(referenceState.second, deltaState);
                }
            }

            auto snapshot = _getSnapshot(tick, mSnapshots, true);
            if(snapshot->confirmed) throw od::Exception("Re-committing snapshot");
            *snapshot = std::move(*incomingSnapshot);
            mIncomingSnapshots.erase(incomingSnapshot);

            if(mUplinkConnectorForAck != nullptr)
            {
                mUplinkConnectorForAck->acknowledgeSnapshot(tick);
            }
        }
    }

    void StateManager::_throwIfStateUpdatesDisallowed()
    {
        if(mDisallowStateUpdates)
        {
            throw od::Exception("State updates disallowed while applying states");
        }
    }

}
