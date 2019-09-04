/*
 * IpAddress.cpp
 *
 *  Created on: Sep 3, 2019
 *      Author: zal
 */

#include <odCore/net/IpAddress.h>
#include <sstream>

namespace odNet
{

    IpParseException::IpParseException(const std::string &msg)
    : od::Exception(msg)
    {
    }


    IpV4Address::IpV4Address(const std::array<int, 4> &addr)
    : mAddress(addr)
    {
    }

    IpV4Address::IpV4Address(const char *addrStr)
    {
        std::istringstream iss(addrStr);
        for(size_t i = 0; i < 4; ++i)
        {
            iss >> mAddress[i];
            if(iss.fail()) throw IpParseException(addrStr);

            if((i < 3) && (iss.get() != '.')) throw IpParseException(addrStr);
        }
    }

}


