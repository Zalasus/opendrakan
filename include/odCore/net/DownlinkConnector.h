/*
 * @file DownlinkConnector.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_NET_DOWNLINKCONNECTOR_H_
#define INCLUDE_ODCORE_NET_DOWNLINKCONNECTOR_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/IdTypes.h>
#include <odCore/ObjectStates.h>

#include <odCore/state/Timeline.h>

namespace odNet
{

    /**
     * @brief Interface representing the server-to-client communication.
     *
     * This is an abstraction of the server-to-client tunnel. A client always
     * provides an implementation of this. For local connections, that object
     * can be used to connect local servers to the client with minimal overhead.
     * For network-based connections, that DownlinkConnector can be used by the
     * packet-parsing end of a network tunnel.
     *
     * Keep in mind that everything which passes through this interface will
     * eventually have to pass through network packets, so keep passed data as
     * simple as possible!
     */
    class DownlinkConnector
    {
    public:

        virtual ~DownlinkConnector() = default;

        virtual void loadLevel(const std::string &path) = 0;

        virtual void objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states) = 0;

        virtual void spawnObject(od::LevelObjectId id) = 0;
        virtual void despawnObject(od::LevelObjectId id) = 0;
        virtual void destroyObject(od::LevelObjectId id) = 0;

        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount) = 0;
    };

}

#endif
