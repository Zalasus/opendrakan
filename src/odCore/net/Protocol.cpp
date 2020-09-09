
#include <odCore/net/Protocol.h>

namespace odNet
{

    size_t getObjectStateChangePacketSize(uint32_t flags)
    {
        size_t size = PacketConstants::STATE_HEADER_SIZE;

        if(flags & PacketConstants::STATE_MASK_POSITION)   size += 3*4; // vec3 translation;
        if(flags & PacketConstants::STATE_MASK_ROTATION)   size += 4*4; // quat rotation;
        if(flags & PacketConstants::STATE_MASK_SCALE)      size += 3*4; // vec3 scale;
        if(flags & PacketConstants::STATE_MASK_VISIBILITY) size += 1;   // u8 visibility;

        return size;
    }

}
