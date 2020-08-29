/*
 * @file StateManager.h
 * @date 21 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_STATE_STATEMANAGER_H_
#define INCLUDE_ODCORE_STATE_STATEMANAGER_H_

#include <mutex>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/ObjectTransform.h>

#include <odCore/input/Keys.h>

#include <odCore/state/Event.h>
#include <odCore/state/State.h>
#include <odCore/state/Timeline.h>

namespace od
{
    class Level;
    class LevelObject;
}

namespace odNet
{
    class ClientConnector;
}

namespace odState
{

    class StateManager
    {
    public:

        StateManager(od::Level &level);

        //ChangeSet &getUpdateLoopChangeSet();
        //ChangeSet &getIncomingChangeSet(TickNumber tick);

        // these modify the update loop changeset (for server)
        void objectTransformed(od::LevelObject &object, const od::ObjectTransform &tf);
        void objectVisibilityChanged(od::LevelObject &object, bool visible);
        void objectCustomStateChanged(od::LevelObject &object);

        // these modify the incoming changeset with the given tick (for client)
        void incomingObjectTransformed(TickNumber tick, od::LevelObject &object, const od::ObjectTransform &tf);
        void incomingObjectVisibilityChanged(TickNumber tick, od::LevelObject &object, bool visible);
        void incomingObjectCustomStateChanged(TickNumber tick, od::LevelObject &object);
        void confirmIncomingSnapshot(TickNumber tick, double time, size_t changeCount);

        /**
         * @brief Finalizes the staging snapshot, merges it into the base state and moves it into the timeline.
         *
         * Server only.
         */
        void commit(double realtime);

        void apply(double realtime);

        /**
         * @brief Applies the given tick to the level with optional forward interpolation.
         *
         * Use this carefully! The applied state will stay forever. Most likely,
         * checkout(...) is what you want to do instead.
         *
         * This supports interpolation between ticks. The lerp parameter can be
         * choosen in the range [0, inf), where 0 is exactly at the passed tick
         * and 1 is at the next tick.
         * TODO: dead reckoning can be implemented here, too, but we'd need to look back more than one tick (or use the base state)
         *
         * Will throw if the given tick number is not being held in memory.
         */
        void apply(TickNumber tick, float lerp = 0.0);

        /**
         * @brief Sends all actions and state transitions of the latest snapshot to the client connector.
         *
         * Will throw if there is no commited snapshot in the timeline.
         */
        void sendLatestSnapshotToClient(odNet::ClientConnector &c);


    private:

        friend class ApplyGuard;

        using ChangeMap = std::unordered_map<od::LevelObjectId, ObjectStateChange>;

        struct Snapshot
        {
            Snapshot(TickNumber t)
            : tick(t)
            , realtime(0.0)
            , discreteChangeCount(0)
            , targetDiscreteChangeCount(0)
            , confirmed(false)
            {
            }

            Snapshot(Snapshot &&s) = default;
            Snapshot(const Snapshot &s) = delete;
            Snapshot &operator=(Snapshot &&) = default;

            ChangeMap changesSinceLastSnapshot;
            TickNumber tick;
            double realtime;
            size_t discreteChangeCount;

            size_t targetDiscreteChangeCount;
            bool confirmed;
        };

        using SnapshotIterator = std::deque<Snapshot>::iterator;

        /**
         * @brief Searches for a snapshot with the given tick. If none exists, creates one at the appropriate position.
         */
        SnapshotIterator _getSnapshot(TickNumber tick, std::deque<Snapshot> &snapshots);
        void _commitIncomingIfComplete(TickNumber tick, SnapshotIterator incomingSnapshot);

        od::Level &mLevel;

        bool mIgnoreStateUpdates;

        /**
         * A set of state events that can take the level as loaded from the file
         * to the most recent committed snapshot. This is useful for new joining
         * clients as well as for creating savegames.
         *
         * This is basically a union of all state changes that happened so far.
         */
        ChangeMap mBaseStateChangeMap;

        ChangeMap mCurrentUpdateChangeMap;

        std::deque<Snapshot> mSnapshots;
        std::mutex mSnapshotMutex; // this synchronized accesses to mSnapshots

        std::deque<Snapshot> mIncomingSnapshots;
    };

}

#endif /* INCLUDE_ODCORE_STATE_STATEMANAGER_H_ */
