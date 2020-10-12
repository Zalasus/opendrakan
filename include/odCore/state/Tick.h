
#ifndef INCLUDE_ODCORE_STATE_TICK_H_
#define INCLUDE_ODCORE_STATE_TICK_H_

#include <odCore/CTypes.h>

namespace odState
{

    using TickNumber = std::int64_t;

    // having a signed tick number makes some operations in the timeline less troublesome. protocol-wise, it is unsigned
    static_assert(std::is_signed<TickNumber>::value, "TickNumber must be of a signed type");

    constexpr TickNumber INVALID_TICK = 0;
    constexpr TickNumber FIRST_TICK = 1;

}

#endif
