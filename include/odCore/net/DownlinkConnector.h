/*
 * @file DownlinkConnector.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_NET_DOWNLINKCONNECTOR_H_
#define INCLUDE_ODCORE_NET_DOWNLINKCONNECTOR_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/CTypes.h>
#include <odCore/IdTypes.h>
#include <odCore/ObjectStates.h>

#include <odCore/db/Asset.h>
#include <odCore/db/IdTypes.h>

#include <odCore/net/IdTypes.h>

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

        virtual void globalDatabaseTableEntry(odDb::GlobalDatabaseIndex dbIndex, const std::string &path) = 0;
        virtual void loadLevel(const std::string &path, size_t loadedDatabaseCount) = 0;

        virtual void objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states) = 0;
        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount, odState::TickNumber referenceTick) = 0;

        /**
         * Extra states are passed through this interface in serialized form. Since extra states are abstract, many parts of the network tunnel
         * would have to deal with allocating the right objects to deserialize the bundle, which seems more of a hassle than forcing a rather simple
         * serialization.
         */
        virtual void objectExtraStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const char *data, size_t size) = 0;

        virtual void globalMessage(MessageChannelCode code, const char *data, size_t size) = 0;
        //virtual void objectMessage(MessageChannelCode code, od::LevelObjectId sender, od::LevelObjectId receiver, const char *data, size_t size) = 0;

        virtual void objectAnimation(od::LevelObjectId id, odDb::GlobalAssetRef animRef, int32_t channelIndex, float speedModifier, double realtime) = 0;
    };

}

#endif
