/*
 * @file StateManager.h
 * @date 21 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_STATE_STATEMANAGER_H_
#define INCLUDE_ODCORE_STATE_STATEMANAGER_H_

#include <unordered_map>
#include <deque>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/IdTypes.h>
#include <odCore/ObjectStates.h>

#include <odCore/input/Keys.h>

#include <odCore/state/Tick.h>

namespace od
{
    class Level;
    class LevelObject;
}

namespace odNet
{
    class DownlinkConnector;
    class UplinkConnector;
}

namespace odState
{

    class StateManager
    {
    public:

        StateManager(od::Level &level);

        /**
         * @brief Assigns an uplink connector to which snapshot acknowledgements will be reported. May be nullptr.
         */
        void setUplinkConnector(std::shared_ptr<odNet::UplinkConnector> c);

        TickNumber getLatestTick();

        /**
         * @brief Returns the realtime of the latest snapshot, or 0.0 if no snapshots exist yet.
         */
        double getLatestRealtime();

        // these modify the update loop changeset (for server)
        void objectStatesChanged(od::LevelObject &obj, const od::ObjectStates &newStates);
        void objectExtraStatesChanged(od::LevelObject &object, const StateBundleBase &states);

        // these modify the incoming changeset with the given tick (for client)
        void incomingObjectStatesChanged(TickNumber tick, od::LevelObjectId objectId, const od::ObjectStates &states);
        void incomingObjectExtraStatesChanged(TickNumber tick, od::LevelObjectId objectId, const char *data, size_t size);
        void confirmIncomingSnapshot(TickNumber tick, double time, size_t changeCount, TickNumber referenceSnapshot);

        /**
         * @brief Finalizes the staging snapshot and moves it into the timeline.
         *
         * Server only.
         */
        void commit(double realtime);

        void apply(double realtime);

        /**
         * @brief Sends all actions and state transitions of the snapshot with the given tick to the given downlink.
         *
         * Will throw if no committed snapshot with that tick exists in the timeline.
         *
         * This will perform delta-encoding against the snapshot with tick
         * referenceSnapshot if that snapshot is still in the timeline. This
         * will usually be the the tick of the latest snapshot whose reception a
         * client acknowledged. Pass INVALID_TICK to force sending a full
         * snapshot. If the snapshot is no longer being held in memory, a full
         * snapshot will be sent as well.
         *
         * @param referenceSnapshot  The tick number to be used for delta encoding (INVALID_TICK if no delta-encoding desired).
         */
        void sendSnapshotToClient(TickNumber tickToSend, odNet::DownlinkConnector &c, TickNumber referenceSnapshot);


    private:

        friend class ApplyGuard;

        struct CombinedStates
        {
            size_t countStatesWithValue() const;
            void merge(const CombinedStates &lhs, const CombinedStates &rhs);
            void lerp(const CombinedStates &lhs, const CombinedStates &rhs, float delta);
            void deltaEncode(const CombinedStates &reference, const CombinedStates &toEncode);

            void makeExtraStatesUnique();
            void applyToObject(od::LevelObject &obj);

            od::ObjectStates basicStates;
            std::shared_ptr<StateBundleBase> extraStates;
        };

        using StatesMap = std::unordered_map<od::LevelObjectId, CombinedStates>;

        struct Snapshot
        {
            Snapshot(TickNumber t)
            : tick(t)
            , realtime(0.0)
            , targetDiscreteChangeCount(0)
            , confirmed(false)
            {
            }

            Snapshot(Snapshot &&s) = default;
            Snapshot(const Snapshot &s) = delete;
            Snapshot &operator=(Snapshot &&) = default;

            StatesMap statesMap;
            TickNumber tick;
            double realtime;

            // bookkeeping for incoming snapshots. unused otherwise
            size_t targetDiscreteChangeCount;
            bool confirmed;
            TickNumber referenceSnapshot;
        };

        using SnapshotIterator = std::deque<Snapshot>::iterator;

        /**
         * @brief Searches for a snapshot with the given tick. Optionally creates one at the appropriate position.
         */
        SnapshotIterator _getSnapshot(TickNumber tick, std::deque<Snapshot> &snapshots, bool createIfNotFound);
        void _commitIncomingIfComplete(TickNumber tick, SnapshotIterator incomingSnapshot);
        void _throwIfStateUpdatesDisallowed();

        od::Level &mLevel;

        bool mDisallowStateUpdates;

        /**
         * During the update loop, all changes first go here. This map is never
         * cleared, so after every update loop, this represents a full snapshot.
         *
         * On both clients and servers, this is only used locally.
         */
        StatesMap mCurrentUpdateStatesMap;

        std::deque<Snapshot> mSnapshots;

        /**
         * A list of incoming snapshots. The client uses this to store changes
         * coming from the server. Since server packets can arrive out of order,
         * snapshots are kept here until they are complete.
         *
         * In contrast to all other snapshots in the StateManager, these are
         * "delta-encoded", i.e. they only list changes that happened since a
         * given reference snapshot (the last one that was acknowledged).
         */
        std::deque<Snapshot> mIncomingSnapshots;

        std::shared_ptr<odNet::UplinkConnector> mUplinkConnectorForAck;

        std::vector<char> mExtraStateSerializationBuffer;
    };

}

#endif /* INCLUDE_ODCORE_STATE_STATEMANAGER_H_ */
