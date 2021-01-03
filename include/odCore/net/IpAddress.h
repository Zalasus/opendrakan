/*
 * IpAddress.h
 *
 *  Created on: Sep 3, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_NET_IPADDRESS_H_
#define INCLUDE_ODCORE_NET_IPADDRESS_H_

#include <array>

#include <odCore/Exception.h>

namespace odNet
{

    class IpParseException : public od::Exception
    {
    public:

        IpParseException(const std::string &msg);

    };

    class IpV4Address
    {
    public:

        IpV4Address(const std::array<int, 4> &addr);
        IpV4Address(const char *addrStr);


    private:

        std::array<int, 4> mAddress;
    };

}


#endif /* INCLUDE_ODCORE_NET_IPADDRESS_H_ */
