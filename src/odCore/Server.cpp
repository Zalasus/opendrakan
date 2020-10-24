/*
 * Server.cpp
 *
 *  Created on: Sep 26, 2019
 *      Author: zal
 */

#include <odCore/Server.h>

#include <thread>
#include <chrono>

#include <odCore/Level.h>

#include <odCore/net/UplinkConnector.h>
#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/MessageDispatcher.h>

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/RflManager.h>

#include <odCore/state/StateManager.h>

#include <odCore/input/InputManager.h>

namespace od
{

    LagCompensationGuard::LagCompensationGuard(odState::StateManager &sm, double rollbackTime)
    : mStateManager(&sm)
    , mRollbackTime(rollbackTime)
    {
    }

    LagCompensationGuard::LagCompensationGuard(LagCompensationGuard &&g)
    : mStateManager(g.mStateManager)
    , mRollbackTime(g.mRollbackTime)
    {
        g.mStateManager = nullptr;
    }

    LagCompensationGuard::~LagCompensationGuard()
    {
        if(mStateManager != nullptr)
        {
            mStateManager->apply(mRollbackTime);
        }
    }


    class LocalUplinkConnector : public odNet::UplinkConnector
    {
    public:

        LocalUplinkConnector(Server &server, Server::ClientData &client)
        : mServer(server)
        , mClient(client)
        {
        }

        virtual void actionTriggered(odInput::ActionCode code, odInput::ActionState state) override
        {
            mClient.inputManager->injectAction(code, state);
        }

        virtual void analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes) override
        {
            mClient.inputManager->injectAnalogAction(code, axes);
        }

        virtual void acknowledgeSnapshot(odState::TickNumber tick) override
        {
            if(tick > mClient.lastAcknowledgedTick)
            {
                mClient.lastAcknowledgedTick = tick;
            }
        }


    private:

        Server &mServer;
        Server::ClientData &mClient;

    };


    Server::Server(odDb::DbManager &dbManager, odRfl::RflManager &rflManager)
    : mDbManager(dbManager)
    , mRflManager(rflManager)
    , mIsDone(false)
    , mNextClientId(1)
    {
        mPhysicsSystem = std::make_unique<odBulletPhysics::BulletPhysicsSystem>(nullptr);
    }

    Server::~Server()
    {
    }

    odNet::ClientId Server::addClient()
    {
        std::lock_guard<std::mutex> lock(mClientsMutex);

        odNet::ClientId newClientId = mNextClientId++;

        auto clientData = std::make_unique<ClientData>();
        clientData->uplinkConnector = std::make_shared<odNet::QueuedUplinkConnector>();
        clientData->inputManager = std::make_unique<odInput::InputManager>();
        clientData->messageDispatcher = std::make_unique<odNet::DownlinkMessageDispatcher>();

        mClients[newClientId] = std::move(clientData);

        return newClientId;
    }

    void Server::setClientDownlinkConnector(odNet::ClientId id, std::shared_ptr<odNet::DownlinkConnector> connector)
    {
        auto &client = _getClientData(id);

        client.downlinkConnector = connector;
        client.messageDispatcher->setDownlinkConnector(connector);
    }

    std::shared_ptr<odNet::QueuedUplinkConnector> Server::getUplinkConnectorForClient(odNet::ClientId clientId)
    {
        return _getClientData(clientId).uplinkConnector;
    }

    odInput::InputManager &Server::getInputManagerForClient(odNet::ClientId id)
    {
        return *(_getClientData(id).inputManager);
    }

    odNet::DownlinkMessageDispatcher &Server::getMessageDispatcherForClient(odNet::ClientId id)
    {
        return *(_getClientData(id).messageDispatcher);
    }

    LagCompensationGuard Server::compensateLag(odNet::ClientId id)
    {
        double predictedClientViewTime = mServerTime - getEstimatedClientLag(id);

        mStateManager->apply(predictedClientViewTime);

        return { *mStateManager, mServerTime };
    }

    float Server::getEstimatedClientLag(odNet::ClientId id)
    {
        auto &client = _getClientData(id);

        return client.lastMeasuredRoundTripTime/2 - client.viewInterpolationTime;
    }

    void Server::loadLevel(const FilePath &lvlPath)
    {
        Logger::verbose() << "Server loading level " << lvlPath;

        Engine engine(*this);

        mLevel = std::make_unique<Level>(engine);
        mLevel->loadLevel(lvlPath.adjustCase(), mDbManager);

        mStateManager = std::make_unique<odState::StateManager>(*mLevel);

        mLevel->spawnAllObjects();

        // in order for clients to be able to load the level, we have to give them
        //  a level path that does not depend on the engine root. for levels that are
        //  stored under the engine root, we can just trim off that part of the path.
        //  for out-of-tree-levels (not under engine root) we have no way to tell the
        //  client where they might find that particular file on their system, so we
        //  fall back to providing the full path, which will work fine for local clients.
        //  networked clients will probably not be used with out-of-tree-levels, anyway.
        auto relLevelPath = lvlPath.removePrefix(getEngineRootDir()).str();

        // copy clients into temporary vector of pointers which we don't have to synchronize (to prevent deadlocks on recursive accesses to clients)
        //  TODO: this is duplicate code, see update loop
        {
            std::lock_guard<std::mutex> lock(mClientsMutex);
            mTempClientUpdateList.clear();
            mTempClientUpdateList.reserve(mClients.size());
            for(auto &client : mClients)
            {
                mTempClientUpdateList.push_back(client.second.get());
            }
        }

        for(auto client : mTempClientUpdateList)
        {
            if(client->downlinkConnector != nullptr)
            {
                client->downlinkConnector->loadLevel(relLevelPath);
            }
        }

        mRflManager.forEachLoadedRfl([this](odRfl::Rfl &rfl){ rfl.onLevelLoaded(*this); });
    }

    void Server::run()
    {
        Logger::info() << "OpenDrakan server starting...";

        Logger::info() << "Server set up. Starting main server loop";

        double targetUpdateIntervalNs = (1e9/60.0);
        auto targetUpdateInterval = std::chrono::nanoseconds((int64_t)targetUpdateIntervalNs);
        auto lastUpdateStartTime = std::chrono::high_resolution_clock::now();
        mServerTime = 0.0;
        while(!mIsDone.load(std::memory_order_relaxed))
        {
            auto loopStart = std::chrono::high_resolution_clock::now();

            double relTime = 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(loopStart - lastUpdateStartTime).count();
            lastUpdateStartTime = loopStart;

            mServerTime += relTime;

            if(mLevel != nullptr)
            {
                mLevel->update(relTime);
            }

            mPhysicsSystem->update(relTime);

            // copy clients into temporary vector of pointers which we don't have to synchronize (to prevent deadlocks on recursive accesses to clients)
            {
                std::lock_guard<std::mutex> lock(mClientsMutex);
                mTempClientUpdateList.clear();
                mTempClientUpdateList.reserve(mClients.size());
                for(auto &client : mClients)
                {
                    mTempClientUpdateList.push_back(client.second.get());
                }
            }

            // update per-client subsystems and process received packets
            for(auto client : mTempClientUpdateList)
            {
                LocalUplinkConnector localConnector(*this, *client);
                client->uplinkConnector->flushQueue(localConnector);

                client->inputManager->update(relTime);
            }

            // commit update
            mStateManager->commit(mServerTime);

            // send update to clients
            odState::TickNumber latestTick = mStateManager->getLatestTick();
            for(auto client : mTempClientUpdateList)
            {
                if(latestTick < client->nextTickToSend)
                {
                    continue;
                }

                if(client->downlinkConnector != nullptr)
                {
                    mStateManager->sendSnapshotToClient(latestTick, *client->downlinkConnector, client->lastAcknowledgedTick);
                }

                // for now, send with fixes rate. later, we'd likely adapt the rate with which we send snapshots based on the client's network speed
                client->nextTickToSend = latestTick+3;
            }

            auto loopEnd = std::chrono::high_resolution_clock::now();
            auto loopTime = loopEnd - loopStart;
            if(loopTime < targetUpdateInterval)
            {
                std::this_thread::sleep_for(targetUpdateInterval - loopTime);

            }else
            {
                float loopTimeMs = 1e-3 * std::chrono::duration_cast<std::chrono::microseconds>(loopTime).count();
                Logger::warn() << "Server tick took too long (" << loopTimeMs << "ms, target was " << (targetUpdateIntervalNs*1e-6) << "ms)";
            }
        }

        Logger::info() << "Shutting down server gracefully";
    }

    Server::ClientData &Server::_getClientData(odNet::ClientId id)
    {
        std::lock_guard<std::mutex> lock(mClientsMutex);

        auto it = mClients.find(id);
        if(it == mClients.end())
        {
            throw od::NotFoundException("Invalid client ID");
        }

        return *it->second;
    }

    Server::ClientData::ClientData()
    : nextTickToSend(odState::FIRST_TICK)
    , lastAcknowledgedTick(odState::INVALID_TICK)
    , viewInterpolationTime(0.1) // TODO: use constant or communicate via handshake
    , lastMeasuredRoundTripTime(0.0)
    {
    }

}
