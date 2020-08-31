/*
 * @file ClientConnector.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_NET_CLIENTCONNECTOR_H_
#define INCLUDE_ODCORE_NET_CLIENTCONNECTOR_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/IdTypes.h>

#include <odCore/state/Timeline.h>

namespace odNet
{

    /**
     * @brief A client ID that uniquely identifies a client
     */
    using ClientId = uint32_t;

    /**
     * @brief Server-side interface for connecting a client to the local server.
     *
     * This is an abstraction of the server-to-client tunnel. Implementations
     * could either use a network connection as a transport medium, or directly
     * connect a local server to a local client with minimal overhead.
     *
     * Keep in mind that everything which passes through this interface will
     * eventually have to pass through network packets, so keep passed data as
     * simple as possible!
     */
    class ClientConnector
    {
    public:

        virtual ~ClientConnector() = default;

        virtual void loadLevel(const std::string &path) = 0;

        virtual void objectTranslated(odState::TickNumber tick, od::LevelObjectId id, const glm::vec3 &p) = 0;
        virtual void objectRotated(odState::TickNumber tick, od::LevelObjectId id, const glm::quat &q) = 0;
        virtual void objectScaled(odState::TickNumber tick, od::LevelObjectId id, const glm::vec3 &s) = 0;
        virtual void objectVisibilityChanged(odState::TickNumber tick, od::LevelObjectId id, bool visible) = 0;

        virtual void spawnObject(od::LevelObjectId id) = 0;
        virtual void despawnObject(od::LevelObjectId id) = 0;
        virtual void destroyObject(od::LevelObjectId id) = 0;

        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount) = 0;
    };

}

#endif
