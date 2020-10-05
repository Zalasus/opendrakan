/*
 * Socket.h
 *
 *  Created on: Sep 3, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_NET_SOCKET_H_
#define INCLUDE_ODCORE_NET_SOCKET_H_

#include <odCore/CTypes.h>
#include <odCore/DataStream.h>

namespace odNet
{
    class IpV4Address;

    class UdpSocket
    {
    public:

        UdpSocket();

        void bind(const IpV4Address &address, uint16_t port);

        od::DataReader getReaderForNextPacket();
    };

}

#endif /* INCLUDE_ODCORE_NET_SOCKET_H_ */
