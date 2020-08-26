/*
 * @file ServerConnector.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_NET_SERVERCONNECTOR_H_
#define INCLUDE_ODCORE_NET_SERVERCONNECTOR_H_

#include <odCore/input/Action.h>

namespace odNet
{

    /**
     * @brief Client-side interface for connecting a server to the local client.
     *
     * This is an abstraction of the client-to-server tunnel. Implementations
     * could either use a network connection as a transport medium, or directly
     * connect a local server to a local client with minimal overhead.
     *
     * Keep in mind that everything which passes through this interface will
     * eventually have to pass through network packets, so keep passed data as
     * simple as possible!
     */
    class ServerConnector
    {
    public:

        virtual ~ServerConnector() = default;

        virtual void actionTriggered(odInput::ActionCode code, odInput::ActionState state) = 0;
        virtual void analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes) = 0;

    };

}

#endif
