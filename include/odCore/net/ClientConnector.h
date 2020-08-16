/*
 * @file ClientConnector.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_NET_CLIENTCONNECTOR_H_
#define INCLUDE_ODCORE_NET_CLIENTCONNECTOR_H_

#include <future>

#include <odCore/IdTypes.h>

#include <odCore/state/ObjectTransform.h>

namespace odNet
{

    /**
     * @brief A client ID that uniquely identifies a client
     */
    using ClientId = uint32_t;

    enum class CommandResult
    {
        ACK,
        NACK,
        TIMEOUT
    };

    /**
     * @brief Server-side interface for connecting a client to the server.
     *
     * This is an abstraction of the client-server tunnel, specifically of the
     * server-to-client direction. Implementations could either use a network
     * connection as a transport medium, or directly connect a local server to
     * a local client with minimal overhead.
     *
     * Keep in mind that everything which passes through this interface will
     * eventually have to pass through network packets, so keep passed data as
     * simple as possible!
     */
    class ClientConnector
    {
    public:

        virtual ~ClientConnector() = default;

        virtual std::future<CommandResult> loadLevel(const std::string &path) = 0;

        virtual std::future<CommandResult> levelObjectTranformed(od::LevelObjectId id, const odState::ObjectTransform &tf) = 0;
        virtual std::future<CommandResult> objectVisibilityChanged(od::LevelObjectId id, bool visible) = 0;

        virtual std::future<CommandResult> spawnObject(od::LevelObjectId id) = 0;
        virtual std::future<CommandResult> despawnObject(od::LevelObjectId id) = 0;
        virtual std::future<CommandResult> destroyObject(od::LevelObjectId id) = 0;
    };

}

#endif
