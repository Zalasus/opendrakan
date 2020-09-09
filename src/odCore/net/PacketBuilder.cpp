
#include <odCore/net/PacketBuilder.h>

#include <algorithm>
#include <type_traits>

namespace odNet
{

    DownlinkPacketBuilder::DownlinkPacketBuilder(const std::function<void(const char *, size_t)> &packetCallback)
    : mPacketCallback(packetCallback)
    , mStreamBuffer(nullptr, 0)
    , mOutputStream(&mStreamBuffer)
    , mWriter(mOutputStream)
    {
    }

    void DownlinkPacketBuilder::loadLevel(const std::string &path)
    {
        uint16_t pathLength = path.size() > 0xffff ? 0xffff : path.size(); // TODO: warn on truncation?

        _beginPacket(PacketType::LOAD_LEVEL, pathLength);
        mWriter.write(path.data(), pathLength);
        _endPacket();
    }

    void DownlinkPacketBuilder::objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states)
    {
        uint32_t flags = 0;
        if(states.position.hasValue()) flags |= PacketConstants::STATE_MASK_POSITION;
        if(states.rotation.hasValue()) flags |= PacketConstants::STATE_MASK_ROTATION;
        if(states.scale.hasValue()) flags |= PacketConstants::STATE_MASK_SCALE;
        if(states.visibility.hasValue()) flags |= PacketConstants::STATE_MASK_VISIBILITY;

        size_t length = getObjectStateChangePacketSize(flags);
        if(length > 0xffff) throw od::Exception("Object State Change packet would be too big");
        uint16_t payloadLength = static_cast<uint16_t>(length);

        _beginPacket(PacketType::OBJECT_STATE_CHANGED, payloadLength);
        mWriter << id
                << flags;
        if(states.position.hasValue()) mWriter << states.position;
        if(states.rotation.hasValue()) mWriter << states.rotation;
        if(states.scale.hasValue()) mWriter << states.scale;
        if(states.visibility.hasValue()) mWriter << states.visibility;
        _endPacket();
    }

    void DownlinkPacketBuilder::spawnObject(od::LevelObjectId id)
    {
    }

    void DownlinkPacketBuilder::despawnObject(od::LevelObjectId id)
    {
    }

    void DownlinkPacketBuilder::destroyObject(od::LevelObjectId id)
    {
    }

    void DownlinkPacketBuilder::confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount)
    {
        _beginPacket(PacketType::CONFIRM_SNAPSHOT, PacketConstants::CONFIRM_PAYLOAD_SIZE);
        mWriter << tick
                << realtime
                << discreteChangeCount;
        _endPacket();
    }

    void DownlinkPacketBuilder::_beginPacket(PacketType type, uint16_t payloadSize)
    {
        mPacketBuffer.resize(payloadSize + PacketConstants::HEADER_SIZE);
        mStreamBuffer = od::MemoryOutputBuffer(mPacketBuffer.data(), mPacketBuffer.size());
        mOutputStream.rdbuf(&mStreamBuffer);

        mWriter << static_cast<uint8_t>(type) << payloadSize;
    }

    void DownlinkPacketBuilder::_endPacket()
    {
        if(mPacketCallback != nullptr)
        {
            mPacketCallback(mPacketBuffer.data(), mPacketBuffer.size());
        }
    }


}
