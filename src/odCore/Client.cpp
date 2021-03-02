/*
 * Client.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: zal
 */

#include <odCore/Client.h>

#include <chrono>
#include <cmath>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/db/Database.h>

#include <odCore/render/Renderer.h>

#include <odCore/input/InputManager.h>
#include <odCore/input/RawActionListener.h>

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

#include <odCore/net/UplinkConnector.h>
#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/MessageDispatcher.h>

#include <odCore/state/StateManager.h>
#include <odCore/state/EventQueue.h>

namespace od
{

    class TranslateEventAssetRefVisitor
    {
    public:

        TranslateEventAssetRefVisitor(Client &client)
        : mClient(client)
        {
        }

        void operator()(odState::Event &e)
        {
        }

        void operator()(odState::ObjectAnimEvent &e)
        {
            e.animRef.globalDbIndex = mClient.translateGlobalDatabaseIndex(e.animRef.globalDbIndex);
        }


    private:

        Client &mClient;

    };


    /**
     * @brief DownlinkConnector for connecting the local client to a server.
     */
    class LocalDownlinkConnector final : public odNet::DownlinkConnector
    {
    public:

        LocalDownlinkConnector(Client &client)
        : mClient(client)
        {
        }

        virtual void globalDatabaseTableEntry(odDb::GlobalDatabaseIndex dbIndex, const std::string &path) override
        {
            FilePath absPath(path, mClient.getEngineRootDir());

            auto db = mClient.getDbManager().getDatabaseByPath(absPath);
            if(db != nullptr)
            {
                mClient.mGlobalDbIndexMap[dbIndex] = db->getGlobalIndex();

            }else
            {
                Logger::error() << "Server database table contains DB '" << path << "', which we have not loaded";
            }
        }

        virtual void loadLevel(const std::string &path, size_t loadedDatabaseCount) override
        {
            mClient.mGlobalDbIndexMap.reserve(loadedDatabaseCount);

            od::FilePath lvlPath(path, mClient.getEngineRootDir());
            mClient.loadLevel(lvlPath);
        }

        virtual void objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states) override
        {
            mClient.getStateManager().incomingObjectStatesChanged(tick, id, states);
        }

        virtual void objectExtraStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const char *data, size_t size) override
        {
            mClient.getStateManager().incomingObjectExtraStatesChanged(tick, id, data, size);
        }

        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount, odState::TickNumber referenceTick) override
        {
            mClient.getStateManager().confirmIncomingSnapshot(tick, realtime, discreteChangeCount, referenceTick);
        }

        virtual void globalMessage(odNet::MessageChannelCode code, const char *data, size_t size) override
        {
            mClient.getMessageDispatcher().receiveGlobalMessage(code, data, size);
        }

        virtual void event(const odState::EventVariant &e, double realtime) override
        {
            auto translatedEvent = e;
            TranslateEventAssetRefVisitor visitor(mClient);
            std::visit(visitor, translatedEvent);

            mClient.getEventQueue().addIncomingEvent(realtime, translatedEvent);
        }

        //virtual void objectMessage(MessageChannelCode code, od::LevelObjectId sender, od::LevelObjectId receiver, const char *data, size_t size) = 0;


    private:

        Client &mClient;

    };



    Client::Client(odDb::DbManager &dbManager, odRfl::RflManager &rflManager, odRender::Renderer &renderer, odAudio::SoundSystem *soundSystem)
    : mDbManager(dbManager)
    , mRflManager(rflManager)
    , mRenderer(renderer)
    , mSoundSystem(soundSystem)
    , mEngineRoot(".")
    , mIsDone(false)
    {
        mPhysicsSystem = std::make_unique<odBulletPhysics::BulletPhysicsSystem>(&renderer);
        mInputManager = std::make_unique<odInput::InputManager>();

        mDownlinkConnector = std::make_shared<odNet::QueuedDownlinkConnector>();

        mMessageDispatcher = std::make_unique<odNet::UplinkMessageDispatcher>();

        mActionListener = mInputManager->createRawActionListener();
        mActionListener->callback = [this](odInput::ActionCode code, odInput::ActionState state)
        {
            if(this->mAssignedUplinkConnector != nullptr)
            {
                this->mAssignedUplinkConnector->actionTriggered(code, state);
            }
        };

        mActionListener->analogCallback = [this](odInput::ActionCode code, const glm::vec2 &axes)
        {
            if(this->mAssignedUplinkConnector != nullptr)
            {
                this->mAssignedUplinkConnector->analogActionTriggered(code, axes);
            }
        };
    }

    Client::~Client()
    {
    }

    void Client::setUplinkConnector(std::shared_ptr<odNet::UplinkConnector> connector)
    {
        mAssignedUplinkConnector = connector;
        mMessageDispatcher->setUplinkConnector(connector);

        if(mStateManager != nullptr)
        {
            mStateManager->setUplinkConnector(connector);
        }
    }

    void Client::loadLevel(const FilePath &lvlPath)
    {
        Logger::verbose() << "Client loading level " << lvlPath;

        Engine engine(*this);

        mLevel = std::make_unique<Level>(engine);
        mLevel->loadLevel(lvlPath.adjustCase(), mDbManager);

        // TODO: all the subsytems whose existence depends on the level could be very elegantly moved to the level class itself
        mStateManager = std::make_unique<odState::StateManager>(*mLevel);
        mStateManager->setUplinkConnector(mAssignedUplinkConnector);

        mEventQueue = std::make_unique<odState::EventQueue>(mDbManager, *mLevel);

        mLevel->spawnAllObjects();
    }

    void Client::run()
    {
        Logger::info() << "OpenDrakan client starting...";

        LocalDownlinkConnector localDownlinkConnector(*this);

        mRenderer.setup();

        Logger::info() << "Client set up. Starting render loop";

        float lerpTime = 0.1;
        float timeTolerance = 0.05;

        double clientTime = 0;
        auto lastUpdateStartTime = std::chrono::high_resolution_clock::now();
        while(!mIsDone.load(std::memory_order_relaxed))
        {
            auto loopStart = std::chrono::high_resolution_clock::now();
            double relTime = 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(loopStart - lastUpdateStartTime).count();
            lastUpdateStartTime = loopStart;

            clientTime += relTime;
            if(mEventQueue != nullptr)
            {
                mEventQueue->setCurrentTime(clientTime);
            }

            mDownlinkConnector->flushQueue(localDownlinkConnector);

            if(mStateManager != nullptr)
            {
                mStateManager->beginClientTick();
            }

            if(mLevel != nullptr)
            {
                mLevel->update(relTime);
            }

            mPhysicsSystem->update(relTime);
            mInputManager->update(relTime);

            if(mStateManager != nullptr)
            {
                // check if we are still in sync before advancing the world
                double latestServerTime = mStateManager->getLatestRealtime();
                if(latestServerTime > clientTime + lerpTime + timeTolerance)
                {
                    Logger::info() << "Client resyncing (servertime=" << latestServerTime << " clienttime=" << clientTime << ")";
                    clientTime = latestServerTime - lerpTime;
                }

                mStateManager->apply(clientTime, false);
            }

            if(mEventQueue != nullptr)
            {
                mEventQueue->dispatch(clientTime);
                mEventQueue->cleanup();
            }

            mRenderer.frame(relTime);
        }

        Logger::info() << "Shutting down client gracefully";
    }

    odDb::GlobalDatabaseIndex Client::translateGlobalDatabaseIndex(odDb::GlobalDatabaseIndex serverSideIndex)
    {
        auto it = mGlobalDbIndexMap.find(serverSideIndex);
        if(it == mGlobalDbIndexMap.end())
        {
            OD_PANIC() << "Server-side global DB index " << serverSideIndex << " not found in table. Server must have sent incomplete table";
        }

        return it->second;
    }

}
