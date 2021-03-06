
#ifndef INCLUDE_ODCORE_NET_PROTOCOL_H_
#define INCLUDE_ODCORE_NET_PROTOCOL_H_

#include <odCore/CTypes.h>

namespace odNet
{

    enum class PacketType : uint8_t
    {
        GLOBAL_DB_TABLE_ENTRY,
        LOAD_LEVEL,

        OBJECT_STATES_CHANGED,
        OBJECT_EXTRA_STATES_CHANGED,
        CONFIRM_SNAPSHOT,
        GLOBAL_MESSAGE,
        OBJECT_MESSAGE,
        
        EVENT,

        ACKNOWLEDGE_SNAPSHOT,
        ACTION_TRIGGERED,
        ANALOG_ACTION_TRIGGERED
    };

    struct PacketConstants
    {
        static constexpr size_t HEADER_SIZE = 3;
        static constexpr size_t EXTRA_STATES_HEADER_SIZE = 12;
        static constexpr size_t GLOBAL_MESSAGE_HEADER_SIZE = 4;
    };

}

#endif
