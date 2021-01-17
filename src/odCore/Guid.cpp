/*
 * Guid.cpp
 *
 *  Created on: Jun 15, 2019
 *      Author: zal
 */

#include <odCore/Guid.h>

#include <cstring>
#include <iomanip>
#include <sstream>

#include <odCore/Panic.h>

namespace od
{

    Guid::Guid(RiffReader &rr)
    {
        if(rr.getChunkLength() != Guid::LENGTH)
        {
            OD_PANIC() << "Can't load GUID from RIFF chunk because it has an invalid length";
        }

        od::DataReader dr = rr.getDataReader();
        dr.read(data.data(), 16);
    }

    bool Guid::operator<(const Guid &rhs) const
    {
        return std::memcmp(data.data(), rhs.data.data(), LENGTH) < 0;
    }

    bool Guid::operator==(const Guid &rhs) const
    {
        return std::memcmp(data.data(), rhs.data.data(), LENGTH) == 0;
    }

    std::ostream &operator<<(std::ostream &lhs, const Guid &rhs)
    {
        std::ostringstream ss;
        ss << std::hex;
        for(size_t i = 0; i < Guid::LENGTH; ++i)
        {
            ss << std::setfill('0') << std::setw(2) << ((uint32_t)rhs.data[i] & 0xff);
        }
        ss << std::dec;

        lhs << ss.str();

        return lhs;
    }

}
