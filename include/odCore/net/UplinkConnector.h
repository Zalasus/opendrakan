/*
 * @file UplinkConnector.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_NET_UPLINKCONNECTOR_H_
#define INCLUDE_ODCORE_NET_UPLINKCONNECTOR_H_

#include <glm/vec2.hpp>

#include <odCore/input/Action.h>

namespace odNet
{

    /**
     * @brief Interface representing the client-to-server communication.
     *
     * This is an abstraction of the client-to-server tunnel. A server always
     * provides an implementation of this. For local connections, that object
     * can be used to connect local clients to the server with minimal overhead.
     * For network-based connections, that UplinkConnector can be used by the
     * packet-parsing end of a network tunnel.
     *
     * Keep in mind that everything which passes through this interface will
     * eventually have to pass through network packets, so keep passed data as
     * simple as possible!
     */
    class UplinkConnector
    {
    public:

        virtual ~UplinkConnector() = default;

        virtual void actionTriggered(odInput::ActionCode code, odInput::ActionState state) = 0;
        virtual void analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes) = 0;

    };

}

#endif
