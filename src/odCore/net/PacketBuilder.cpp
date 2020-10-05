
#include <odCore/net/PacketBuilder.h>

#include <algorithm>
#include <type_traits>

namespace odNet
{

    DownlinkPacketBuilder::DownlinkPacketBuilder(const std::function<void(const char *, size_t)> &packetCallback)
    : mPacketCallback(packetCallback)
    , mStreamBuffer(mPacketBuffer)
    , mOutputStream(&mStreamBuffer)
    , mWriter(mOutputStream)
    {
    }

    void DownlinkPacketBuilder::loadLevel(const std::string &path)
    {
        uint16_t pathLength = path.size() > 0xffff ? 0xffff : path.size(); // TODO: warn on truncation?

        _beginPacket(PacketType::LOAD_LEVEL);
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

        _beginPacket(PacketType::OBJECT_STATE_CHANGED);
        mWriter << id
                << flags;
        if(states.position.hasValue())   mWriter << states.position.get();
        if(states.rotation.hasValue())   mWriter << states.rotation.get();
        if(states.scale.hasValue())      mWriter << states.scale.get();
        if(states.visibility.hasValue()) mWriter << (states.visibility.get() ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0));
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
        _beginPacket(PacketType::CONFIRM_SNAPSHOT);
        mWriter << tick
                << realtime
                << discreteChangeCount;
        _endPacket();
    }

    void DownlinkPacketBuilder::globalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        _beginPacket(PacketType::GLOBAL_MESSAGE);
        mWriter << code;
        mWriter.write(data, size);
        _endPacket();
    }

    void DownlinkPacketBuilder::_beginPacket(PacketType type)
    {
        mPacketBuffer.clear();
        //mOutputStream.seekp(0);

        uint16_t dummyPayloadSize = 0;
        mWriter << static_cast<uint8_t>(type) << dummyPayloadSize;
    }

    void DownlinkPacketBuilder::_endPacket()
    {
        mOutputStream.flush();

        size_t payloadSize = mPacketBuffer.size() - PacketConstants::HEADER_SIZE;
        if(payloadSize > 0xffff)
        {
            throw od::Exception("Packet payload size exceeds size fields limits");
        }

        //mOutputStream.seekp(1);
        //mWriter << static_cast<uint16_t>(payloadSize);

        // FIXME: bad hack! while seeking is unimplemented, we have to overwrite the payload size field manually
        mPacketBuffer[1] = (payloadSize >> 0) & 0xff;
        mPacketBuffer[2] = (payloadSize >> 8) & 0xff;

        if(mPacketCallback != nullptr)
        {
            mPacketCallback(mPacketBuffer.data(), mPacketBuffer.size());
        }
    }


}
