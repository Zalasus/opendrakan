
#include <odCore/state/StateManager.h>

#include <algorithm>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/net/ClientConnector.h>

namespace odState
{

    static const TickNumber TICK_CAPACITY = 16;


    struct ApplyChangeToObjectVisitor
    {
        od::LevelObject &obj;

        ApplyChangeToObjectVisitor(od::LevelObject &o)
        : obj(o)
        {
        }

        void translated(const glm::vec3 &p)
        {
            obj.setPosition(p);
        }

        void rotated(const glm::quat &r)
        {
            obj.setRotation(r);
        }

        void scaled(const glm::vec3 &s)
        {
            obj.setScale(s);
        }

        void visibilityChanged(bool b)
        {
            obj.setVisible(b);
        }

        void animationFrameChanged(float t)
        {
        }
    };


    struct ApplyChangeToClientConnectorVisitor
    {
        ApplyChangeToClientConnectorVisitor(TickNumber tick, odNet::ClientConnector &c)
        {
        }

        void translated(const glm::vec3 &p)
        {
        }

        void rotated(const glm::quat &r)
        {
        }

        void scaled(const glm::vec3 &s)
        {
        }

        void visibilityChanged(bool b)
        {
        }

        void animationFrameChanged(float t)
        {
        }
    };


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

    void StateManager::objectTransformed(od::LevelObject &object, const od::ObjectTransform &tf)
    {
        if(mIgnoreStateUpdates) return;

        auto &objectChanges = mCurrentUpdateChangeMap[object.getObjectId()];
        if(tf.isTranslation()) objectChanges.setTranslation(tf.getPosition());
        if(tf.isRotation()) objectChanges.setRotation(tf.getRotation());
        if(tf.isScaling()) objectChanges.setScale(tf.getScale());
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
        auto snapshot = _getSnapshot(tick, mIncomingSnapshots, true);
        auto &objectChanges = snapshot->changes[object.getObjectId()];

        // TODO: aaa
        //if(!objectChanges.isTransformed()) snapshot->discreteChangeCount++;
        //objectChanges.setTransform(tf);

        _commitIncomingIfComplete(tick, snapshot);
    }

    void StateManager::incomingObjectVisibilityChanged(TickNumber tick, od::LevelObject &object, bool visible)
    {
        if(mIgnoreStateUpdates) return;
        auto snapshot = _getSnapshot(tick, mIncomingSnapshots, true);
        auto &objectChanges = snapshot->changes[object.getObjectId()];

        if(!objectChanges.isVisibilityChanged()) snapshot->discreteChangeCount++;
        objectChanges.setVisibility(visible);

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

                ApplyChangeToObjectVisitor applyVisitor(*obj);
                objChange.second.visit(applyVisitor);
            }

        }else if(it == mSnapshots.begin())
        {
            // we only have one snapshot in the timeline, and it's later than the requested time.
            //  extrapolating here is probably unnecessary, so we just apply the snapshot as if it happened right now.
            for(auto &objChange : it->changes)
            {
                od::LevelObject *obj = mLevel.getLevelObjectById(objChange.first);
                if(obj == nullptr) continue;

                ApplyChangeToObjectVisitor applyVisitor(*obj);
                objChange.second.visit(applyVisitor);
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

                ApplyChangeToObjectVisitor applyVisitor(*obj);

                auto changeInB = b.changes.find(objChange.first);
                if(changeInB == b.changes.end())
                {
                    // no corresponding change in B. this should not happen, as
                    //  all snapshots reflect all changes since load. for now, assume steady state
                    objChange.second.visit(applyVisitor);

                }else
                {
                    // TODO: a flag indicating that a state has not changed since the last snapshot might improve performance a tiny bit by ommitting the lerp.
                    //  we still have to store full snapshots, as we must move through the timeline arbitrarily, and for every missing state, we'd have to
                    //  search previous and intermediate snapshots to recover the original state
                    auto lerped = objChange.second.lerp(changeInB->second, delta);
                    lerped.visit(applyVisitor);
                }
            }
        }
    }

    void StateManager::sendSnapshotToClient(TickNumber tickToSend, odNet::ClientConnector &c, TickNumber lastSentSnapshot)
    {
        std::lock_guard<std::mutex> lock(mSnapshotMutex);

        auto toSend = _getSnapshot(tickToSend, mSnapshots, false);
        if(toSend == mSnapshots.end())
        {
            throw od::Exception("Snapshot with given tick not available for sending");
        }

        size_t discreteChangeCount = 0;
        ApplyChangeToClientConnectorVisitor applyVisitor(tickToSend, c);

        auto lastSent = _getSnapshot(lastSentSnapshot, mSnapshots, false);

        for(auto &stateChange : toSend->changes)
        {
            ObjectStateChange filteredChange = stateChange.second;
            if(lastSent != mSnapshots.end())
            {
                auto prevChange = lastSent->changes.find(stateChange.first);
                if(prevChange != lastSent->changes.end())
                {
                    filteredChange.removeSteadyStates(prevChange->second);
                }
            }

            filteredChange.visit(applyVisitor);

            discreteChangeCount += filteredChange.getDiscreteChangeCount();
        }

        c.confirmSnapshot(tickToSend, toSend->realtime, discreteChangeCount);
    }

    StateManager::SnapshotIterator StateManager::_getSnapshot(TickNumber tick, std::deque<Snapshot> &snapshots, bool createIfNotFound)
    {
        auto pred = [](Snapshot &snapshot, TickNumber tick) { return snapshot.tick == tick; };
        auto it = std::lower_bound(snapshots.begin(), snapshots.end(), tick, pred);

        if(it == snapshots.end())
        {
            if(createIfNotFound)
            {
                return snapshots.emplace(snapshots.begin(), tick);

            }else
            {
                return it;
            }

        }else if(it->tick == tick)
        {
            return it;

        }else
        {
            if(createIfNotFound)
            {
                return snapshots.emplace(it, tick);

            }else
            {
                return mSnapshots.end();
            }
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

            // TODO: merge incoming with previous full snapshot
            //  FIXME: due to out-of-order transport, an incoming snapshot might get confirmed before the one it's delta encoding depends on. take that into account!!

            auto snapshot = _getSnapshot(tick, mSnapshots, true);
            if(snapshot->confirmed) throw od::Exception("Re-committing snapshot");
            *snapshot = std::move(*incomingSnapshot);
            mIncomingSnapshots.erase(incomingSnapshot);
        }
    }

}
