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

        static const TickNumber FIRST_TICK = 1;


        StateManager(od::Level &level);

        TickNumber getLatestTick();

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
         * @brief Finalizes the staging snapshot and moves it into the timeline.
         *
         * Server only.
         */
        void commit(double realtime);

        void apply(double realtime);

        /**
         * @brief Sends all actions and state transitions of the snapshot with the given tick to the client connector.
         *
         * Will throw if no committed snapshot with that tick exists in the timeline.
         *
         * TODO: add option to send a full snapshot (other than passing lastSentSnapshot = FIRST_TICK - 1)
         *
         * @param lastSentSnapshot  The tick number of the last snapshot that was sent to the client (for delta-encoding).
         */
        void sendSnapshotToClient(TickNumber tickToSend, odNet::ClientConnector &c, TickNumber lastSentSnapshot);


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

            ChangeMap changes;
            TickNumber tick;
            double realtime;

            // bookkeeping for incoming snapshots
            //  TODO: move out of this struct, as they are only ever valid for incoming snapshots
            size_t discreteChangeCount;
            size_t targetDiscreteChangeCount;
            bool confirmed;
        };

        using SnapshotIterator = std::deque<Snapshot>::iterator;

        /**
         * @brief Searches for a snapshot with the given tick. Optionally creates one at the appropriate position.
         */
        SnapshotIterator _getSnapshot(TickNumber tick, std::deque<Snapshot> &snapshots, bool createIfNotFound);
        void _commitIncomingIfComplete(TickNumber tick, SnapshotIterator incomingSnapshot);

        od::Level &mLevel;

        bool mIgnoreStateUpdates;

        /**
         * During the update loop, all changes first go here. This map is never
         * cleared, so after every update loop, this represents a full snapshot.
         *
         * On both clients and servers, this is only used locally.
         */
        ChangeMap mCurrentUpdateChangeMap;

        std::deque<Snapshot> mSnapshots;
        std::mutex mSnapshotMutex; // this synchronized accesses to mSnapshots

        /**
         * A list of incoming snapshots. The client uses this to store changes
         * coming from the server. Since server packets can arrive out of order,
         * snapshots are kept here until they are complete.
         *
         * In contrast to all other snapshots in the StateManager, these are
         * "delta-encoded", i.e. they only list changes that happened since the
         * last snapshot.
         */
        std::deque<Snapshot> mIncomingSnapshots;
    };

}

#endif /* INCLUDE_ODCORE_STATE_STATEMANAGER_H_ */
