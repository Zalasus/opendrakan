/*
 * @file Engine.h
 * @author zal
 */

#ifndef INCLUDE_ENGINE_H_
#define INCLUDE_ENGINE_H_

#include <cassert>

#include <odCore/FilePath.h>

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

namespace odState
{
    class StateManager;
}

namespace od
{
    class Client;
    class Server;

    /**
     * @brief Variant for either an od::Client or an od::Server.
     *
     * For convenience, this provides access to subsystems both Client and
     * Server can provide.
     *
     * This is strictly a *variant*. It is *not* meant as a container for both
     * Client and Server at the same time even if both happen to exist in the
     * same process like during startup or in a singleplayer game.
     *
     * TODO: why not make this an interface that Client and Server implement instead?
     */
    class Engine
    {
    public:

        Engine(Client &client);
        Engine(Server &server);

        inline bool isServer() const { _assertValid(); return mServer != nullptr; }
        inline bool isClient() const { _assertValid(); return mClient != nullptr; }

        od::Client &getClient();
        od::Server &getServer();

        const od::FilePath &getEngineRootDir() const;
        odDb::DbManager &getDbManager();
        odRfl::RflManager &getRflManager();
        odPhysics::PhysicsSystem &getPhysicsSystem();
        odState::StateManager &getStateManager();


    private:

        inline void _assertValid() const
        {
            assert((mClient != nullptr) != (mServer != nullptr));
        }

        Client *mClient;
        Server *mServer;

    };

}

#endif
