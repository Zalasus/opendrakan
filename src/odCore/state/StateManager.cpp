
#include <odCore/state/StateManager.h>

#include <algorithm>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Panic.h>

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
        _panicIfStateUpdatesDisallowed();

        auto &storedStates = mCurrentUpdateStatesMap[object.getObjectId()].basicStates;
        storedStates.merge(storedStates, newStates);
    }

    void StateManager::objectExtraStatesChanged(od::LevelObject &object, const StateBundleBase &newStates)
    {
        _panicIfStateUpdatesDisallowed();

        auto &storedStates = mCurrentUpdateStatesMap[object.getObjectId()].extraStates;

        if(storedStates == nullptr || storedStates.use_count() > 1)
        {
            storedStates = newStates.cloneShared();
        }

        // storedStates is nonnull and unique
        storedStates->merge(*storedStates, newStates);
    }

    void StateManager::incomingObjectStatesChanged(TickNumber tick, od::LevelObjectId objectId, const od::ObjectStates &newStates)
    {
        auto snapshotIt = _getSnapshot(tick, mIncomingSnapshots, true);
        auto &states = snapshotIt->statesMap[objectId].basicStates;
        states.assign(newStates);
        _commitIncomingIfComplete(tick, snapshotIt);
    }

    void StateManager::incomingObjectExtraStatesChanged(TickNumber tick, od::LevelObjectId objectId, const char *data, size_t size)
    {
        auto snapshotIt = _getSnapshot(tick, mIncomingSnapshots, true);

        auto &states = snapshotIt->statesMap[objectId].extraStates;
        if(states == nullptr)
        {
            auto obj = mLevel.getLevelObjectById(objectId);
            if(obj == nullptr || obj->getExtraStates() == nullptr)
            {
                OD_PANIC() << "No states available to clone";
            }

            states = obj->getExtraStates()->cloneShared();

        }else if(states.use_count() > 1)
        {
            states = states->cloneShared();
            states->clear();
        }

        states->clear();

        od::MemoryInputBuffer buffer(data, size);
        std::istream stream(&buffer);
        od::DataReader reader(stream);

        states->deserialize(reader, odState::StateSerializationPurpose::NETWORK);

        _commitIncomingIfComplete(tick, snapshotIt);
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
            oldSnapshot.statesMap = mCurrentUpdateStatesMap;
            mSnapshots.emplace_back(std::move(oldSnapshot));

        }else
        {
            mSnapshots.emplace_back(nextTick);
            auto &newSnapshot = mSnapshots.back();
            newSnapshot.statesMap = mCurrentUpdateStatesMap;
            newSnapshot.realtime = realtime;
        }
    }

    void StateManager::apply(double realtime, bool full)
    {
        ApplyGuard applyGuard(*this);

        // TODO: not all states change everytime! we should remember the last tick we applied and only process states that changed between the two.
        //  right now we apply *every* tracked state every single frame even if it didn't change, and that wastes a lot of time
        // note that there might be cases where that behaviour is desired (like reverting changes not made by the state manager)

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
            for(auto &states : mSnapshots.back().statesMap)
            {
                auto obj = mLevel.getLevelObjectById(states.first);
                if(obj == nullptr) continue;

                _applyToObject(states.second, *obj);
            }

        }else if(it == mSnapshots.begin())
        {
            // we only have one snapshot in the timeline, and it's later than the requested time.
            //  extrapolating here is probably unnecessary, so we just apply the snapshot as if it happened right now.
            for(auto &states : it->statesMap)
            {
                auto obj = mLevel.getLevelObjectById(states.first);
                if(obj == nullptr) continue;

                _applyToObject(states.second, *obj);
            }

        }else
        {
            Snapshot &a = *(it-1);
            Snapshot &b = *it;

            double delta = (realtime - a.realtime)/(b.realtime - a.realtime);

            for(auto &states : a.statesMap)
            {
                auto obj = mLevel.getLevelObjectById(states.first);
                if(obj == nullptr) continue;

                auto stateInB = b.statesMap.find(states.first);
                if(stateInB == b.statesMap.end())
                {
                    // no corresponding change in B. this should not happen, as
                    //  all snapshots reflect all changes since load. for now, assume steady state
                    Logger::warn() << "Incomplete timeline. A tracked state seems to have disappeared";
                    _applyToObject(states.second, *obj);

                }else
                {
                    // TODO: a flag indicating that a state has not changed since the last snapshot might improve performance a tiny bit by ommitting the lerp.
                    //  we still have to store full snapshots, as we must move through the timeline arbitrarily, and for every missing state, we'd have to
                    //  search previous and intermediate snapshots to recover the original state
                    CombinedStates lerped;
                    lerped.lerp(states.second, stateInB->second, delta);
                    _applyToObject(lerped, *obj);
                }
            }
        }
    }

    void StateManager::beginClientTick()
    {
        mCurrentUpdateStatesMap.clear();
    }

    void StateManager::sendSnapshotToClient(TickNumber tickToSend, odNet::DownlinkConnector &c, TickNumber referenceSnapshot)
    {
        auto toSend = _getSnapshot(tickToSend, mSnapshots, false);
        if(toSend == mSnapshots.end())
        {
            OD_PANIC() << "Snapshot with given tick not available for sending";
        }

        size_t discreteChangeCount = 0;

        auto reference = (referenceSnapshot != INVALID_TICK) ? _getSnapshot(referenceSnapshot, mSnapshots, false) : mSnapshots.end();

        for(auto &states : toSend->statesMap)
        {
            CombinedStates encodedState = states.second;
            if(reference != mSnapshots.end())
            {
                auto referenceState = reference->statesMap.find(states.first);
                if(referenceState != reference->statesMap.end())
                {
                    encodedState.deltaEncode(referenceState->second, encodedState);
                }
            }

            size_t basicChangeCount = encodedState.basicStates.countStatesWithValue();
            if(basicChangeCount > 0)
            {
                c.objectStatesChanged(tickToSend, states.first, encodedState.basicStates);
            }

            size_t extraChangeCount = (encodedState.extraStates != nullptr) ? encodedState.extraStates->countStatesWithValue() : 0;
            if(extraChangeCount > 0)
            {
                mExtraStateSerializationBuffer.clear();
                od::VectorOutputBuffer buf(mExtraStateSerializationBuffer);
                std::ostream out(&buf);
                od::DataWriter writer(out);
                encodedState.extraStates->serialize(writer, odState::StateSerializationPurpose::NETWORK);

                c.objectExtraStatesChanged(tickToSend, states.first, mExtraStateSerializationBuffer.data(), mExtraStateSerializationBuffer.size());
            }

            discreteChangeCount += basicChangeCount + extraChangeCount;
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
        for(auto &states : incomingSnapshot->statesMap)
        {
            discreteChangeCount += states.second.countStatesWithValue();
        }

        if(incomingSnapshot->targetDiscreteChangeCount == discreteChangeCount)
        {
            // this snapshot is complete! move it to the timeline
            if(mSnapshots.size() >= TICK_CAPACITY)
            {
                // TODO: implement this as a ring buffer or something so we don't loose the allocated extra StateBundles
                mSnapshots.pop_front();
            }

            // undo delta-encoding by merging incoming with the reference snapshot (only if this is not a full snapshot)
            if(incomingSnapshot->referenceSnapshot != INVALID_TICK)
            {
                auto referenceSnapshot = _getSnapshot(incomingSnapshot->referenceSnapshot, mSnapshots, false);
                if(referenceSnapshot == mSnapshots.end())
                {
                    OD_PANIC() << "Reference snapshot no longer contained in timeline";
                }

                for(auto &referenceStates : referenceSnapshot->statesMap)
                {
                    auto &deltaStates = incomingSnapshot->statesMap[referenceStates.first];
                    deltaStates.merge(referenceStates.second, deltaStates);
                }
            }

            auto snapshot = _getSnapshot(tick, mSnapshots, true);
            if(snapshot->confirmed) OD_PANIC() << "Re-committing snapshot";
            *snapshot = std::move(*incomingSnapshot);
            mIncomingSnapshots.erase(incomingSnapshot);

            if(mUplinkConnectorForAck != nullptr)
            {
                mUplinkConnectorForAck->acknowledgeSnapshot(tick);
            }

        }else
        {
            Logger::warn() << incomingSnapshot->targetDiscreteChangeCount << " > " << discreteChangeCount;
        }
    }

    void StateManager::_panicIfStateUpdatesDisallowed()
    {
        if(mDisallowStateUpdates)
        {
            OD_PANIC() << "State updates disallowed while applying states";
        }
    }

    void StateManager::_applyToObject(const StateManager::CombinedStates &states, od::LevelObject &obj)
    {
        if(obj.isStatePredictionEnabled())
        {
            // unimplemented. for now, apply server states only if the client
            //  made no updates to the object this tick.
            auto predicted = mCurrentUpdateStatesMap.find(obj.getObjectId());
            if(predicted == mCurrentUpdateStatesMap.end())
            {
                obj.setStatesUntracked(states.basicStates);
                if(states.extraStates != nullptr)
                {
                    obj.setExtraStatesUntracked(*states.extraStates);
                }
            }

        }else
        {
            // delta-encode with current states so we don't re-apply unchanged states
            //  TODO: there must be a better way of doing this. this is not a real optimization
            od::ObjectStates encoded = states.basicStates;
            encoded.deltaEncode(obj.getStates(), states.basicStates);
            obj.setStatesUntracked(encoded);

            if(states.extraStates != nullptr)
            {
                obj.setExtraStatesUntracked(*states.extraStates);
            }
        }
    }


    size_t StateManager::CombinedStates::countStatesWithValue() const
    {
        return basicStates.countStatesWithValue() + (extraStates != nullptr ? extraStates->countStatesWithValue() : 0);
    }

    void StateManager::CombinedStates::merge(const CombinedStates &lhs, const CombinedStates &rhs)
    {
        basicStates.merge(lhs.basicStates, rhs.basicStates);

        if(extraStates == nullptr)
        {
            if(lhs.extraStates != nullptr && rhs.extraStates != nullptr)
            {
                extraStates = lhs.extraStates->cloneShared();
                extraStates->merge(*lhs.extraStates, *rhs.extraStates);

            }else if(lhs.extraStates != nullptr && rhs.extraStates == nullptr)
            {
                extraStates = lhs.extraStates;

            }else if(lhs.extraStates == nullptr && rhs.extraStates != nullptr)
            {
                extraStates = rhs.extraStates;
            }

        }else
        {
            makeExtraStatesUnique();
            if(lhs.extraStates != nullptr && rhs.extraStates != nullptr)
            {
                extraStates->merge(*lhs.extraStates, *rhs.extraStates);

            }else if(lhs.extraStates != nullptr && rhs.extraStates == nullptr)
            {
                extraStates->assign(*lhs.extraStates);

            }else if(lhs.extraStates == nullptr && rhs.extraStates != nullptr)
            {
                extraStates->assign(*rhs.extraStates);
            }
        }
    }

    void StateManager::CombinedStates::lerp(const CombinedStates &lhs, const CombinedStates &rhs, float delta)
    {
        basicStates.lerp(lhs.basicStates, rhs.basicStates, delta);

        if(extraStates == nullptr)
        {
            if(lhs.extraStates != nullptr && rhs.extraStates != nullptr)
            {
                extraStates = lhs.extraStates->cloneShared();
                extraStates->lerp(*lhs.extraStates, *rhs.extraStates, delta);

            }else if(lhs.extraStates != nullptr && rhs.extraStates == nullptr)
            {
                extraStates = lhs.extraStates;

            }else if(lhs.extraStates == nullptr && rhs.extraStates != nullptr)
            {
                extraStates = rhs.extraStates;
            }

        }else
        {
            makeExtraStatesUnique();
            if(lhs.extraStates != nullptr && rhs.extraStates != nullptr)
            {
                extraStates->lerp(*lhs.extraStates, *rhs.extraStates, delta);

            }else if(lhs.extraStates != nullptr && rhs.extraStates == nullptr)
            {
                extraStates->assign(*lhs.extraStates);

            }else if(lhs.extraStates == nullptr && rhs.extraStates != nullptr)
            {
                extraStates->assign(*rhs.extraStates);
            }
        }
    }

    void StateManager::CombinedStates::deltaEncode(const CombinedStates &reference, const CombinedStates &toEncode)
    {
        basicStates.deltaEncode(reference.basicStates, toEncode.basicStates);

        if(extraStates == nullptr)
        {
            if(reference.extraStates != nullptr && toEncode.extraStates != nullptr)
            {
                extraStates = reference.extraStates->cloneShared();
                extraStates->deltaEncode(*reference.extraStates, *toEncode.extraStates);

            }else if(reference.extraStates != nullptr && toEncode.extraStates == nullptr)
            {
                extraStates = nullptr; // may stay nullptr

            }else if(reference.extraStates == nullptr && toEncode.extraStates != nullptr)
            {
                extraStates = toEncode.extraStates;
            }

        }else
        {
            makeExtraStatesUnique();
            if(reference.extraStates != nullptr && toEncode.extraStates != nullptr)
            {
                extraStates->deltaEncode(*reference.extraStates, *toEncode.extraStates);

            }else if(reference.extraStates != nullptr && toEncode.extraStates == nullptr)
            {
                extraStates->clear();

            }else if(reference.extraStates == nullptr && toEncode.extraStates != nullptr)
            {
                extraStates->assign(*toEncode.extraStates);
            }
        }
    }

    void StateManager::CombinedStates::makeExtraStatesUnique()
    {
        if(extraStates != nullptr && extraStates.use_count() > 1)
        {
            extraStates = extraStates->cloneShared();
        }
    }

}
