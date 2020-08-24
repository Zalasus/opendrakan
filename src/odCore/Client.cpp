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

    static std::future<odNet::CommandResult> _makeFutureFor(odNet::CommandResult r)
    {
        auto task = [r](){ return r; };
        return std::async(std::launch::deferred, task);
    }


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

        virtual std::future<odNet::CommandResult> loadLevel(const std::string &path) override
        {
            od::FilePath lvlPath(path, mClient.getEngineRootDir());
            mClient.loadLevel(lvlPath);
            return _makeFutureFor(odNet::CommandResult::ACK);
        }

        virtual std::future<odNet::CommandResult> levelObjectTranformed(LevelObjectId id, const odState::ObjectTransform &tf) override
        {
            od::LevelObject *obj = getObjectById(id);
            if(obj != nullptr)
            {
                //mClient.getStateManager().addObjectTransform(*obj, tf);
                return _makeFutureFor(odNet::CommandResult::ACK);

            }else
            {
                return _makeFutureFor(odNet::CommandResult::NACK);
            }
        }

        virtual std::future<odNet::CommandResult> objectVisibilityChanged(od::LevelObjectId id, bool visible) override
        {
            od::LevelObject *obj = getObjectById(id);
            if(obj != nullptr)
            {
                //mClient.getStateManager().addObjectVisibilityChange(*obj, visible);
                return _makeFutureFor(odNet::CommandResult::ACK);

            }else
            {
                return _makeFutureFor(odNet::CommandResult::NACK);
            }
        }

        virtual std::future<odNet::CommandResult> spawnObject(od::LevelObjectId id)
        {
            od::LevelObject *obj = getObjectById(id);
            if(obj != nullptr)
            {
                obj->spawned();
                return _makeFutureFor(odNet::CommandResult::ACK);

            }else
            {
                return _makeFutureFor(odNet::CommandResult::NACK);
            }
        }

        virtual std::future<odNet::CommandResult> despawnObject(od::LevelObjectId id)
        {
            od::LevelObject *obj = getObjectById(id);
            if(obj != nullptr)
            {
                obj->despawned();
                return _makeFutureFor(odNet::CommandResult::ACK);

            }else
            {
                return _makeFutureFor(odNet::CommandResult::NACK);
            }
        }

        virtual std::future<odNet::CommandResult> destroyObject(od::LevelObjectId id)
        {
            od::LevelObject *obj = getObjectById(id);
            if(obj != nullptr)
            {
                obj->requestDestruction();
            }

            // can't fail. a non-existing object is (technically) destroyed
            return _makeFutureFor(odNet::CommandResult::ACK);
        }


    private:

        od::LevelObject *getObjectById(od::LevelObjectId id)
        {
            if(mClient.getLevel() == nullptr) return nullptr;

            return mClient.getLevel()->getLevelObjectById(id);
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

            mRenderer.frame(relTime);
        }

        Logger::info() << "Shutting down client gracefully";
    }

}
