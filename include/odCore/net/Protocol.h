
#ifndef INCLUDE_ODCORE_NET_PROTOCOL_H_
#define INCLUDE_ODCORE_NET_PROTOCOL_H_

#include <stdint.h>

namespace odNet
{

    enum class PacketType : uint8_t
    {
        OBJECT_STATE_CHANGED,
        CONFIRM_SNAPSHOT,
        LOAD_LEVEL
    };

    struct PacketConstants
    {
        static constexpr size_t HEADER_SIZE = 3;

        static constexpr size_t   STATE_HEADER_SIZE = 16;
        static constexpr uint32_t STATE_MASK_POSITION   = (1 << 0);
        static constexpr uint32_t STATE_MASK_ROTATION   = (1 << 1);
        static constexpr uint32_t STATE_MASK_SCALE      = (1 << 2);
        static constexpr uint32_t STATE_MASK_VISIBILITY = (1 << 3);

        static constexpr size_t   CONFIRM_PAYLOAD_SIZE = 20;
    };

    size_t getObjectStateChangePacketSize(uint32_t flags);
}

#endif
