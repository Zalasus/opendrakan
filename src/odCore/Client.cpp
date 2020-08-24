/*
 * Client.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: zal
 */

#include <odCore/Client.h>

#include <chrono>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/render/Renderer.h>

#include <odCore/input/InputManager.h>
#include <odCore/input/RawActionListener.h>

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

#include <odCore/net/ClientConnector.h>
#include <odCore/net/ServerConnector.h>

#include <odCore/state/StateManager.h>


namespace od
{

    /**
     * @brief ClientConnector for connecting a local client to the server.
     */
    class LocalClientConnector final : public odNet::ClientConnector
    {
    public:

        LocalClientConnector(Client &client)
        : mClient(client)
        {
        }

        virtual void loadLevel(const std::string &path) override
        {
            od::FilePath lvlPath(path, mClient.getEngineRootDir());
            mClient.loadLevel(lvlPath);
        }

        virtual void objectTransformed(odState::TickNumber tick, LevelObjectId id, const od::ObjectTransform &tf) override
        {
            auto &obj = _getObjectById(id);
            mClient.getStateManager().objectTransformed(obj, tf, tick);
        }

        virtual void objectVisibilityChanged(odState::TickNumber tick, od::LevelObjectId id, bool visible) override
        {
            auto &obj = _getObjectById(id);
            mClient.getStateManager().objectVisibilityChanged(obj, visible, tick);
        }

        virtual void spawnObject(od::LevelObjectId id)
        {
            _getObjectById(id).spawned();
        }

        virtual void despawnObject(od::LevelObjectId id)
        {
            _getObjectById(id).despawned();
        }

        virtual void destroyObject(od::LevelObjectId id)
        {
             _getObjectById(id).requestDestruction();
        }

        virtual void commitTick(odState::TickNumber tick, size_t eventAndStateTransitionCount) override
        {
            // since out-of-order transport is not possible on a local connection,
            //  we can safely assume this always happens after all transitions have been sent
            //if(tick == mClient.getStateManager().getCurrentTick())
            {
                mClient.getStateManager().commit();
                mClient.getStateManager().apply(tick);
            }
        }


    private:

        od::LevelObject &_getObjectById(od::LevelObjectId id)
        {
            if(mClient.getLevel() == nullptr)
            {
                throw od::Exception("No level loaded");
            }

            auto object = mClient.getLevel()->getLevelObjectById(id);
            if(object == nullptr)
            {
                throw od::Exception("Invalid level object ID");
            }

            return *object;
        }

        Client &mClient;

    };



    Client::Client(odDb::DbManager &dbManager, odRfl::RflManager &rflManager, odRender::Renderer &renderer)
    : mDbManager(dbManager)
    , mRflManager(rflManager)
    , mRenderer(renderer)
    , mEngineRoot(".")
    , mIsDone(false)
    {
        mPhysicsSystem = std::make_unique<odBulletPhysics::BulletPhysicsSystem>(&renderer);
        mInputManager = std::make_unique<odInput::InputManager>();

        mActionListener = mInputManager->createRawActionListener();
        mActionListener->callback = [this](odInput::ActionCode code, odInput::ActionState state)
        {
            if(this->mServerConnector != nullptr)
            {
                this->mServerConnector->actionTriggered(code, state);
            }
        };
    }

    Client::~Client()
    {
    }

    std::unique_ptr<odNet::ClientConnector> Client::createLocalConnector()
    {
        return std::make_unique<LocalClientConnector>(*this);
    }

    void Client::setServerConnector(std::unique_ptr<odNet::ServerConnector> connector)
    {
        mServerConnector = std::move(connector);
    }

    void Client::loadLevel(const FilePath &lvlPath)
    {
        Logger::verbose() << "Client loading level " << lvlPath;

        Engine engine(*this);

        mLevel = std::make_unique<Level>(engine);
        mLevel->loadLevel(lvlPath.adjustCase(), mDbManager);

        mStateManager = std::make_unique<odState::StateManager>(*mLevel);

        mLevel->spawnAllObjects();
    }

    void Client::run()
    {
        Logger::info() << "OpenDrakan client starting...";

        mRenderer.setup();

        Logger::info() << "Client set up. Starting render loop";

        using namespace std::chrono;
        auto lastTime = high_resolution_clock::now();
        while(!mIsDone.load(std::memory_order_relaxed))
        {
            auto now = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(now - lastTime);
            lastTime = now;
            float relTime = duration.count() * 1e-6;

            if(mLevel != nullptr)
            {
                mLevel->update(relTime);
            }

            mPhysicsSystem->update(relTime);
            mInputManager->update(relTime);

            // do not call mStateManager->commit() here! the client does not tick by itself, but rather waits for the server to tell it to tick
            //   EDIT: maybe that's not so smart after all. gotta keep thinking about that

            mRenderer.frame(relTime);
        }

        Logger::info() << "Shutting down client gracefully";
    }

}
