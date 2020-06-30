
#include <odCore/Engine.h>

#include <odCore/Client.h>
#include <odCore/Server.h>
#include <odCore/Exception.h>

namespace od
{

    Engine::Engine(Client &client)
    : mClient(&client)
    , mServer(nullptr)
    {
    }

    Engine::Engine(Server &server)
    : mClient(nullptr)
    , mServer(&server)
    {
    }

    Client &Engine::getClient()
    {
        if(mClient == nullptr) throw Exception("Not a client");

        return *mClient;
    }

    Server &Engine::getServer()
    {
        if(mServer == nullptr) throw Exception("Not a server");

        return *mServer;
    }

    const od::FilePath &Engine::getEngineRootDir() const
    {
        _assertValid();

        if(mClient != nullptr)
        {
            return mClient->getEngineRootDir();

        }else if(mServer != nullptr)
        {
            return mServer->getEngineRootDir();

        }else
        {
            throw Exception("Engine was invalid");
        }
    }

    odDb::DbManager &Engine::getDbManager()
    {
        _assertValid();

        if(mClient != nullptr)
        {
            return mClient->getDbManager();

        }else if(mServer != nullptr)
        {
            return mServer->getDbManager();

        }else
        {
            throw Exception("Engine was invalid");
        }
    }

    odRfl::RflManager &Engine::getRflManager()
    {
        _assertValid();

        if(mClient != nullptr)
        {
            return mClient->getRflManager();

        }else if(mServer != nullptr)
        {
            return mServer->getRflManager();

        }else
        {
            throw Exception("Engine was invalid");
        }
    }

    odPhysics::PhysicsSystem &Engine::getPhysicsSystem()
    {
        _assertValid();

        if(mClient != nullptr)
        {
            return mClient->getPhysicsSystem();

        }else if(mServer != nullptr)
        {
            return mServer->getPhysicsSystem();

        }else
        {
            throw Exception("Engine was invalid");
        }
    }

}
