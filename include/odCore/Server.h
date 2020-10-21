/*
 * Server.h
 *
 *  Created on: Sep 24, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_SERVER_H_
#define INCLUDE_ODCORE_SERVER_H_

#include <memory>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <vector>

#include <odCore/FilePath.h>

#include <odCore/net/IdTypes.h>
#include <odCore/net/QueuedUplinkConnector.h>

#include <odCore/state/Timeline.h>

namespace odDb
{
    class DbManager;
}

namespace odRfl
{
    class RflManager;
}

namespace odPhysics
{
    class PhysicsSystem;
}

namespace odInput
{
    class InputManager;
}

namespace odNet
{
    class DownlinkConnector;
    class DownlinkMessageDispatcher;
}

namespace odState
{
    class StateManager;
}

namespace od
{
    class Level;

    class LagCompensationGuard
    {
    public:

        LagCompensationGuard(odState::StateManager &sm, double rollbackTime);
        LagCompensationGuard(LagCompensationGuard &&g);
        ~LagCompensationGuard();


    private:

        odState::StateManager *mStateManager;
        double mRollbackTime;

    };

    /**
     * @brief Local server instance.
     *
     * There is no abstraction between a local and a remote server here! This class solely represents a
     * local server, which can be either a dedicated server, a listen server, or a singleplayer server.
     */
    class Server
    {
    public:

        Server(odDb::DbManager &dbManager, odRfl::RflManager &rflManager);
        ~Server();

        inline void setIsDone(bool b) { mIsDone.store(b, std::memory_order::memory_order_relaxed); }
        inline void setEngineRootDir(const od::FilePath &path) { mEngineRoot = path; }
        inline const od::FilePath &getEngineRootDir() const { return mEngineRoot; }
        inline Level *getLevel() { return mLevel.get(); }

        inline odDb::DbManager &getDbManager() { return mDbManager; }
        inline odRfl::RflManager &getRflManager() { return mRflManager; }
        inline odPhysics::PhysicsSystem &getPhysicsSystem() { return *mPhysicsSystem; }
        inline odState::StateManager &getStateManager() { return *mStateManager; }

        /**
         * @brief Creates a new client and assigns a new client ID to it. It's downlink connector has to be assigned separately.
         * @return the new client ID assigned to the client.
         */
        odNet::ClientId addClient();

        void setClientDownlinkConnector(odNet::ClientId id, std::shared_ptr<odNet::DownlinkConnector> connector);

        /**
         * @brief Returns an uplink connector that can be used to connect the client with the given ID to this server.
         *
         * The client must have already been added to the server via the addClient() method.
         */
        std::shared_ptr<odNet::QueuedUplinkConnector> getUplinkConnectorForClient(odNet::ClientId clientId);

        /**
         * @brief Returns the input manager for the given client.
         *
         * On the server, every connected client has it's own input manager.
         * If the given client ID is not registered, this will throw.
         */
        odInput::InputManager &getInputManagerForClient(odNet::ClientId id);

        odNet::DownlinkMessageDispatcher &getMessageDispatcherForClient(odNet::ClientId id);

        LagCompensationGuard compensateLag(odNet::ClientId id);

        float getEstimatedClientLag(odNet::ClientId id);

        template <typename T>
        void forEachClient(const T &functor)
        {
            for(auto &client : mClients)
            {
                functor(client.first);
            }
        }

        void loadLevel(const FilePath &path);

        void run();


    private:

        friend class LocalUplinkConnector;

        struct ClientData
        {
            ClientData();

            std::shared_ptr<odNet::DownlinkConnector> downlinkConnector;
            std::shared_ptr<odNet::QueuedUplinkConnector> uplinkConnector;
            std::unique_ptr<odInput::InputManager> inputManager;
            std::unique_ptr<odNet::DownlinkMessageDispatcher> messageDispatcher;

            odState::TickNumber nextTickToSend;

            // for synchronizing access to the below fields
            std::mutex mutex;

            // for delta-encoding snapshots
            odState::TickNumber lastAcknowledgedTick;

            // for lag compensation:
            float viewInterpolationTime;
            float lastMeasuredRoundTripTime;
        };

        ClientData &_getClientData(odNet::ClientId id);

        odDb::DbManager &mDbManager;
        odRfl::RflManager &mRflManager;

        std::unique_ptr<odPhysics::PhysicsSystem> mPhysicsSystem;
        std::unique_ptr<Level> mLevel;
        std::unique_ptr<odState::StateManager> mStateManager;

        FilePath mEngineRoot;

        std::atomic_bool mIsDone;

        odNet::ClientId mNextClientId;
        std::unordered_map<odNet::ClientId, std::unique_ptr<ClientData>> mClients;
        std::mutex mClientsMutex; // to synchronize access to clients map (for adding clients etc.). don't hold this when performing actions on clients!

        // updating the clients may cause some of them to cause accesses to the client map. since we have to synchronize
        //  access to that map, this could cause deadlocks if hold the mutex during that time. to prevent this, we only
        //  aquire the mutex for a short time to copy the map into this update list, then iterate over the element without
        //  holding the mutex.
        std::vector<ClientData*> mTempClientUpdateList;

        double mServerTime;

    };

}


#endif /* INCLUDE_ODCORE_SERVER_H_ */
