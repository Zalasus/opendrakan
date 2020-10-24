
#include <odCore/net/PacketBuilder.h>

#include <algorithm>
#include <type_traits>

namespace odNet
{

    PacketBuilder::PacketBuilder(const std::function<void(const char *, size_t, LinkType)> &packetCallback)
    : mPacketCallback(packetCallback)
    , mStreamBuffer(mPacketBuffer)
    , mOutputStream(&mStreamBuffer)
    , mWriter(mOutputStream)
    {
    }

    void PacketBuilder::loadLevel(const std::string &path)
    {
        uint16_t pathLength = path.size() > 0xffff ? 0xffff : path.size(); // TODO: warn on truncation?

        _beginPacket(PacketType::LOAD_LEVEL);
        mWriter.write(path.data(), pathLength);
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states)
    {
        _beginPacket(PacketType::OBJECT_STATE_CHANGED);
        mWriter << tick
                << id;
        states.serialize(mWriter, odState::StateSerializationPurpose::NETWORK);
        _endPacket(LinkType::UNRELIABLE);
    }

    void PacketBuilder::confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount, odState::TickNumber referenceTick)
    {
        _beginPacket(PacketType::CONFIRM_SNAPSHOT);
        mWriter << tick
                << realtime
                << static_cast<uint32_t>(discreteChangeCount)
                << referenceTick;
        _endPacket(LinkType::UNRELIABLE);
    }

    void PacketBuilder::globalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        _beginPacket(PacketType::GLOBAL_MESSAGE);
        mWriter << code;
        mWriter.write(data, size);
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::actionTriggered(odInput::ActionCode code, odInput::ActionState state)
    {
        _beginPacket(PacketType::ACTION_TRIGGERED);
        mWriter << code << static_cast<uint8_t>(state);
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes)
    {
        _beginPacket(PacketType::ANALOG_ACTION_TRIGGERED);
        mWriter << code << axes.x << axes.y;
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::acknowledgeSnapshot(odState::TickNumber tick)
    {
        _beginPacket(PacketType::ACKNOWLEDGE_SNAPSHOT);
        mWriter << tick;
        _endPacket(LinkType::UNRELIABLE);
    }

    void PacketBuilder::_beginPacket(PacketType type)
    {
        mPacketBuffer.clear();

        uint16_t dummyPayloadSize = 0;
        mWriter << static_cast<uint8_t>(type) << dummyPayloadSize;
    }

    void PacketBuilder::_endPacket(LinkType linkType)
    {
        mOutputStream.flush();

        size_t payloadSize = mPacketBuffer.size() - PacketConstants::HEADER_SIZE;
        if(payloadSize > 0xffff)
        {
            throw od::Exception("Packet payload size exceeds size fields limits");
        }

        auto p = mWriter.tell();
        mWriter.seek(1);
        mWriter << static_cast<uint16_t>(payloadSize);
        mWriter.seek(p);

        if(mPacketCallback != nullptr)
        {
            mPacketCallback(mPacketBuffer.data(), mPacketBuffer.size(), linkType);
        }
    }


}
