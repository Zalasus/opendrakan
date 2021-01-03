/*
 * Guid.h
 *
 *  Created on: Jun 15, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_MUSIC_GUID_H_
#define INCLUDE_ODCORE_AUDIO_MUSIC_GUID_H_

#include <array>
#include <ostream>

#include <odCore/RiffReader.h>

namespace od
{

    struct Guid
    {
        static constexpr size_t LENGTH = 16;

        std::array<char, LENGTH> data;

        Guid()
        {
        }

        Guid(const std::array<char, LENGTH> &d)
        : data(d)
        {
        }

        Guid(RiffReader &rr);

        bool operator<(const Guid &rhs) const;
        bool operator==(const Guid &rhs) const;
    };

    std::ostream &operator<<(std::ostream &lhs, const Guid &rhs);

}


#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_GUID_H_ */
