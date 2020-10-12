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

#include <odCore/render/Renderer.h>

#include <odCore/input/InputManager.h>
#include <odCore/input/RawActionListener.h>

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

#include <odCore/net/UplinkConnector.h>
#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/MessageDispatcher.h>

#include <odCore/state/StateManager.h>


namespace od
{

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

        virtual void loadLevel(const std::string &path) override
        {
            od::FilePath lvlPath(path, mClient.getEngineRootDir());
            mClient.loadLevel(lvlPath);
        }

        virtual void objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states) override
        {
            mClient.getStateManager().incomingObjectStatesChanged(tick, id, states);
        }

        virtual void objectLifecycleStateChanged(odState::TickNumber tick, od::LevelObjectId id, od::ObjectLifecycleState state) override
        {
        }

        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount) override
        {
            mClient.getStateManager().confirmIncomingSnapshot(tick, realtime, discreteChangeCount);
        }

        virtual void globalMessage(odNet::MessageChannelCode code, const char *data, size_t size) override
        {
            mClient.getMessageDispatcher().receiveGlobalMessage(code, data, size);
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

        mDownlinkConnector = std::make_shared<LocalDownlinkConnector>(*this);

        mMessageDispatcher = std::make_unique<odNet::UplinkMessageDispatcher>();

        mActionListener = mInputManager->createRawActionListener();
        mActionListener->callback = [this](odInput::ActionCode code, odInput::ActionState state)
        {
            if(this->mUplinkConnector != nullptr)
            {
                this->mUplinkConnector->actionTriggered(code, state);
            }
        };

        mActionListener->analogCallback = [this](odInput::ActionCode code, const glm::vec2 &axes)
        {
            if(this->mUplinkConnector != nullptr)
            {
                this->mUplinkConnector->analogActionTriggered(code, axes);
            }
        };
    }

    Client::~Client()
    {
    }

    void Client::setUplinkConnector(std::shared_ptr<odNet::UplinkConnector> connector)
    {
        mUplinkConnector = connector;
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

        mStateManager = std::make_unique<odState::StateManager>(*mLevel);
        mStateManager->setUplinkConnector(mUplinkConnector);

        mLevel->spawnAllObjects();
    }

    void Client::run()
    {
        Logger::info() << "OpenDrakan client starting...";

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

                mStateManager->apply(clientTime);
            }

            mRenderer.frame(relTime);
        }

        Logger::info() << "Shutting down client gracefully";
    }

}
