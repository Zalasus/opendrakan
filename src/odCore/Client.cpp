/*
 * Client.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: zal
 */

#include <odCore/Client.h>

#include <chrono>

#include <odCore/Level.h>

#include <odCore/render/Renderer.h>

#include <odCore/input/InputManager.h>

#include <odCore/physics/bullet/BulletPhysicsSystem.h>

#include <odCore/net/ClientConnector.h>


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

        virtual std::future<odNet::CommandResult> levelObjectTranformed(LevelObjectId id, const odNet::ObjectTransform &tf) override
        {
            return _makeFutureFor(odNet::CommandResult::ACK);
        }


    private:

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
    }

    Client::~Client()
    {
    }

    std::unique_ptr<odNet::ClientConnector> Client::createLocalConnector()
    {
        return std::make_unique<LocalClientConnector>(*this);
    }

    void Client::loadLevel(const FilePath &lvlPath)
    {
        Logger::verbose() << "Client loading level " << lvlPath;

        Engine engine(*this);

        mLevel = std::make_unique<Level>(engine);
        mLevel->loadLevel(lvlPath.adjustCase(), mDbManager);
        
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
            auto duration = duration_cast<microseconds>(high_resolution_clock::now() - lastTime);
            float relTime = duration.count() * 1e-6;
            mRenderer.frame(relTime);
        }

        Logger::info() << "Shutting down client gracefully";
    }

}
