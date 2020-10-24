
#ifndef INCLUDE_ODCORE_NET_PROTOCOL_H_
#define INCLUDE_ODCORE_NET_PROTOCOL_H_

#include <odCore/CTypes.h>

namespace odNet
{

    enum class PacketType : uint8_t
    {
        OBJECT_STATE_CHANGED,
        CONFIRM_SNAPSHOT,
        LOAD_LEVEL,
        GLOBAL_MESSAGE,
        OBJECT_MESSAGE,

        ACKNOWLEDGE_SNAPSHOT,
        ACTION_TRIGGERED,
        ANALOG_ACTION_TRIGGERED
    };

    struct PacketConstants
    {
        static constexpr size_t HEADER_SIZE = 3;
        static constexpr size_t GLOBAL_MESSAGE_HEADER_SIZE = 4;
    };

}

#endif
