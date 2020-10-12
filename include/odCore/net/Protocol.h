
#ifndef INCLUDE_ODCORE_NET_PROTOCOL_H_
#define INCLUDE_ODCORE_NET_PROTOCOL_H_

#include <odCore/CTypes.h>

namespace odNet
{

    enum class PacketType : uint8_t
    {
        OBJECT_STATE_CHANGED,
        OBJECT_LIFECYCLE_STATE_CHANGED,
        CONFIRM_SNAPSHOT,
        ACKNOWLEDGE_SNAPSHOT,

        LOAD_LEVEL,

        GLOBAL_MESSAGE,
        OBJECT_MESSAGE,

        ACTION_TRIGGERED,
        ANALOG_ACTION_TRIGGERED
    };

    struct PacketConstants
    {
        static constexpr size_t HEADER_SIZE = 3;

        static constexpr size_t   STATE_HEADER_SIZE = 16;
        static constexpr uint32_t STATE_MASK_POSITION   = (1 << 0);
        static constexpr uint32_t STATE_MASK_ROTATION   = (1 << 1);
        static constexpr uint32_t STATE_MASK_SCALE      = (1 << 2);
        static constexpr uint32_t STATE_MASK_VISIBILITY = (1 << 3);

        static constexpr size_t   LIFECYCLE_SIZE = 13;

        static constexpr size_t   CONFIRM_PAYLOAD_SIZE = 28;

        static constexpr size_t   GLOBAL_MESSAGE_HEADER_SIZE = 4;
    };

    size_t getObjectStateChangePacketSize(uint32_t flags);
}

#endif
